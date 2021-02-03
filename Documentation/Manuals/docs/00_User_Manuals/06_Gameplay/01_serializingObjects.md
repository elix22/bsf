---
title: Run-time type information
---

Run-time type information (RTTI) provides meta-data about components (as well as non-component objects). This meta-data allows for features such as dynamic-casting, type-checking and most importantly, serialization.

Often components will have data you will want to persist across application sessions (for example **Renderable** component needs to remember which **Mesh** and **Material** it references). This persistent data will be automatically saved when a scene is saved, and loaded along with the scene. This process is called data serialization.

In order to make an object serializable you need to set up a RTTI interface that allows the system to query information about the object, retrieve and set its data. In this example we talk primarily about components, but the same interface can be used for resources and normal objects.

Any object that is serializable (and therefore provides RTTI information) must implement the @bs::IReflectable interface. If you are creating custom components or resources, **Component** and **Resource** base classes already derive from this interface so you don't need to specify it manually. The interface is simple, requiring you to implement two methods:
 - RTTITypeBase* getRTTI() const;
 - static RTTITypeBase* getRTTIStatic();
 
Implementations of these methods will return an object containing all RTTI for a specific class. In the rest of this manual we'll focus on explaning how to create a RTTI class implementation returned by these methods.

~~~~~~~~~~~~~{.cpp}
// IReflectable implementation for a component
class MyComponent : public Component
{
public:
	MyComponent(const HSceneObject& parent)
		:Component(parent)
	{}

	// ...class members...

	static RTTITypeBase* getRTTIStatic()
	{ return MyComponentRTTI::instance(); }

	RTTITypeBase* getRTTI() const override
	{ return MyComponent::getRTTIStatic(); }
};

// IReflectable implementation for a normal class
class MyClass : public IReflectable
{
	// ...class members...

	static RTTITypeBase* getRTTIStatic()
	{ return MyClassRTTI::instance(); }

	RTTITypeBase* getRTTI() const override
	{ return MyClass::getRTTIStatic(); }
};
~~~~~~~~~~~~~

# Creating the RTTI type
All RTTI objects must implement the @bs::RTTIType<Type, BaseType, MyRTTIType> interface. The interface accepts three template parameters:
 - *Type* - Class of the object we're creating RTTI for (e.g. *MyClass* or *MyComponent* from example above)
 - *BaseType* - Base type of the object we're creating RTTI for (e.g. *IReflectable* or *Component* from example above)
 - *MyRTTIType* - Name of the RTTI class itself
 
~~~~~~~~~~~~~{.cpp}
class MyClassRTTI : public RTTIType<MyClass, IReflectable, MyClassRTTI>
{
	// ...
};

class MyComponentRTTI : public RTTIType<MyComponent, Component, MyComponentRTTI>
{
	// ...
};
~~~~~~~~~~~~~

The RTTI object must at least implement the following methods:
 - @bs::RTTITypeBase::getRTTIName() - Returns the name of the class the RTTI describes
 - @bs::RTTITypeBase::getRTTIId() - Returns an identifier that uniquely identifies the class. This should be a unique integer equal or larger than 200000 (in order to avoid conflict with built-in types).
 - @bs::RTTITypeBase::newRTTIObject() - Creates a new empty instance of the class the RTTI describes
 
~~~~~~~~~~~~~{.cpp}
enum TypeIds
{
	TID_MyClass = 200000,
	TID_MyComponent = 200001
}

class MyClassRTTI : public RTTIType<MyClass, IReflectable, MyClassRTTI>
{
public:
	const String& getRTTIName() override
	{
		static String name = "MyClass";
		return name;
	}

	UINT32 getRTTIId() override
	{
		return TID_MyClass;
	}

	SPtr<IReflectable> newRTTIObject() override
	{
		return bs_shared_ptr_new<MyClass>();
	}
};

class MyComponentRTTI : public RTTIType<MyComponent, Component, MyComponentRTTI>
{
public:
	const String& getRTTIName() override
	{
		static String name = "MyComponent";
		return name;
	}

	UINT32 getRTTIId() override
	{
		return TID_MyComponent;
	}

	SPtr<IReflectable> newRTTIObject() override
	{
		return SceneObject::createEmptyComponent<MyComponent>();
	}
};
~~~~~~~~~~~~~

> Note that when creating new instances of components within the RTTI type class, you must use **SceneObject::createEmptyComponent<T>()** method, instead of just creating a normal shared pointer.

This is the minimal amount of work you need to do in order to implement RTTI. The RTTI types above now describe the class type, but not any of its members. In order to actually have class data serialized, you also need to define member fields.

# Member fields
Member fields give the RTTI type a way to access (retrieve and assign) data from various members in the class the RTTI type describes. Let's imagine our *MyComponent* class had a few data members:
~~~~~~~~~~~~~{.cpp}
class MyComponent : public Component
{
public:
	// ...

	UINT32 myInt;
	float myFloat;
	String myString;

	// ...
};
~~~~~~~~~~~~~

Its field definition within the RTTI type would look like so:
~~~~~~~~~~~~~{.cpp}
class MyComponentRTTI : public RTTIType<MyComponent, Component, MyComponentRTTI>
{
public:
	BS_BEGIN_RTTI_MEMBERS
		BS_RTTI_MEMBER_PLAIN(myInt, 0)
		BS_RTTI_MEMBER_PLAIN(myFloat, 1)
		BS_RTTI_MEMBER_PLAIN(myString, 2)
	BS_END_RTTI_MEMBERS

	// ... 
};
~~~~~~~~~~~~~

Field definition portion of the RTTI type always begins with the @BS_BEGIN_RTTI_MEMBERS macro, and ends with the @BS_END_RTTI_MEMBERS.

The field members themselves are defined by calling macros starting with BS_RTTI_MEMBER_. The macro expects the name of the field it describes, as well as a unique ID of the field. The suffix of the BS_RTTI_MEMBER_ macro depends on the type of the field being added. There are three different types:
 - @BS_RTTI_MEMBER_PLAIN - Field containing basic data types like ints, floats, strings or other types that can be just trivially copied during serialization/deserialization.
 - @BS_RTTI_MEMBER_REFL - Field containing objects deriving from **IReflectable** (i.e. classes that have RTTI). The main advantage of using **IReflectable** types over plain ones is that you are allowed to add or remove fields from **IReflectable** RTTI type, and it wont break any data that was previously serialized. This is very important as you make changes to your components or resources, so you can still load previously saved data (e.g. imagine saving a level, changing a component, and then being unable to load the level). Plain types on the other hand must always keep the same structure, otherwise the serialized data will be broken.
 - @BS_RTTI_MEMBER_REFLPTR - Fields containing pointers to objects deriving from **IReflectable**. Same as **BS_RTTI_MEMBER_REFL**, except that multiple fields can point to the same object. This ensures the object won't be serialized multiple times, wasting space and performance, and also ensures that the system can properly restore all references to the object when it's deserialized.
 
~~~~~~~~~~~~~{.cpp}
// Component definition with more complex field types
class MyComponent : public Component
{
public:
	// ...

	UINT32 myInt;
	float myFloat;
	String myString;
	SPtr<MyClass> myPtrClass;
	MyClass myClass;
	HRenderable renderable; // Component handle
	HMesh mesh; // Resource handle

	// ...
};

class MyComponentRTTI : public RTTIType<MyComponent, Component, MyComponentRTTI>
{
public:
	BS_BEGIN_RTTI_MEMBERS
		BS_RTTI_MEMBER_PLAIN(myInt, 0)
		BS_RTTI_MEMBER_PLAIN(myFloat, 1)
		BS_RTTI_MEMBER_PLAIN(myString, 2)
		BS_RTTI_MEMBER_REFLPTR(myPtrClass, 3)
		BS_RTTI_MEMBER_REFL(myClass, 4)
		BS_RTTI_MEMBER_REFL(renderable, 5)
		BS_RTTI_MEMBER_REFL(mesh, 6)
	BS_END_RTTI_MEMBERS

	// ... 
};
~~~~~~~~~~~~~

> Note that component and resource handles are considered **IReflectable**, and are therefore serialized by using **BS_RTTI_MEMBER_REFL**.

Each field must have an ID unique within the RTTI type. If you remove members from the RTTI type, you should not re-use their IDs for other members. Additionally, if the type of a specific field changes, you should assign it a new ID. The IDs allow the system to map previously serialized data to the current structure of the object.

## Arrays
Array field types are also supported:
 - @BS_RTTI_MEMBER_PLAIN_ARRAY
 - @BS_RTTI_MEMBER_REFL_ARRAY
 - @BS_RTTI_MEMBER_REFLPTR_ARRAY
 
They perform the same function as their non-array counterparts, except the fields are expected to contain a **Vector<T>** instead of a single entry.

~~~~~~~~~~~~~{.cpp}
class MyComponent : public Component
{
public:
	// ...

	Vector<UINT32> myInts;
	Vector<SPtr<MyClass>> myPtrClasses;
	Vector<HMesh> meshes;

	// ...
};

class MyComponentRTTI : public RTTIType<MyComponent, Component, MyComponentRTTI>
{
public:
	BS_BEGIN_RTTI_MEMBERS
		BS_RTTI_MEMBER_PLAIN_ARRAY(myInts, 0)
		BS_RTTI_MEMBER_REFLPTR_ARRAY(myPtrClasses, 1)
		BS_RTTI_MEMBER_REFL_ARRAY(meshes, 2)
	BS_END_RTTI_MEMBERS

	// ... 
};
~~~~~~~~~~~~~

# Using RTTI
Once the RTTI type class has been created, in most cases it will be used automatically. In the case of components it will be used when saving/loading a scene, and in the case of resources it will be used when saving/loading a resource. But for any other class you will want to know how to utilize it manually.

## Manually serializing

To manually serialize an object you can use the @bs::FileEncoder class. Create the file encoder with a path to the output file, followed by a call to @bs::FileEncoder::encode with the object to encode as the parameter. The system will encode the provided object, as well as any other referenced **IReflectable** objects. 

~~~~~~~~~~~~~{.cpp}
SPtr<IReflectable> myObject = bs_shared_ptr_new<MyClass>();

FileEncoder fe("Path\To\My\File.asset");
fe.encode(myObject.get());
~~~~~~~~~~~~~

Once ready to decode, create a @bs::FileDecoder with the same file path. Then call @bs::FileDecoder::decode.

~~~~~~~~~~~~~{.cpp}
FileDecoder fd("Path\To\My\File.asset");
SPtr<IReflectable> myObjectCopy = fd.decode();
~~~~~~~~~~~~~

You can also encode/decode to/from memory by using @bs::BinarySerializer.
~~~~~~~~~~~~~{.cpp}
SPtr<IReflectable> myObject = bs_shared_ptr_new<MyClass>();

BinarySerializer bs;
SPtr<MemoryDataStream> stream = bs_shared_ptr_new<MemoryDataStream>();
bs.encode(myObject.get(), stream);

stream->seek(0);
SPtr<IReflectable> myObjectCopy2 = bs.decode(stream, stream->size());
~~~~~~~~~~~~~

## Casting & queries
Aside from using RTTI for serialization, you can also use it to manually query various information about objects, as well as create and cast object instances. 

Global queries:
 - @bs::rtti_is_of_type - Checks is a specific object of type *T*
 - @bs::rtti_is_subclass - Checks is a specific object derived from type *T*
 - @bs::rtti_create - Creates a new object from its type ID
 - @bs::rtti_cast - Casts an object to the specified type if the cast is valid, or returns null otherwise
 
**IReflectable** queries:
 - @bs::IReflectable::getTypeName - Gets the name of the object's type
 - @bs::IReflectable::getTypeId - Gets the type ID of the object's type

~~~~~~~~~~~~~{.cpp}
IReflectable* myObject = ...;

rtti_is_of_type<Texture>(myObject);
rtti_is_subclass<Texture>(myObject);
rtti_create(TID_Texture);
Texture* myTexture = rtti_cast<Texture>(myObject);

myObject->getTypeName();
myObject->getTypeId();
~~~~~~~~~~~~~

For more information you can also continue reading the [Advanced RTTI](../advancedRtti) manual.
