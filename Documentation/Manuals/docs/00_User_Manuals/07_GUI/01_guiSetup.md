---
title: GUI setup
---

All GUI elements in bs::f are handled by a @bs::CGUIWidget component. Each such component must have an attached **Camera** component, which determines where will the rendered GUI elements be output. 

The camera is created in the same way as we shown before, and you can in-fact use the same camera you use for normal scene rendering. GUI elements will not be affected by camera's position, orientation or projection properties - they might however be affected by the size of the camera's render target.

~~~~~~~~~~~~~{.cpp}
SPtr<RenderWindow> primaryWindow = gApplication().getPrimaryWindow();

HSceneObject cameraSO = SceneObject::create("Camera");
HCamera camera = cameraSO->addComponent<CCamera>(primaryWindow);
~~~~~~~~~~~~~

And we create a **GUIWidget** the same as with any other component.

~~~~~~~~~~~~~{.cpp}
HSceneObject guiSO = SceneObject::create("GUI");
HGUIWidget gui = guiSO->addComponent<CGUIWidget>(camera);
~~~~~~~~~~~~~

Before a widget is usable we must first assign it a **GUISkin**. A skin defines how is every element on this particular widget rendered (its texture(s), fonts, size, etc.). We'll talk about skins and styles in detail later, but for now it's enough to assign the built-in skin available from **BuiltinResources**.

~~~~~~~~~~~~~{.cpp}
gui->setSkin(BuiltinResources::instance().getGUISkin());
~~~~~~~~~~~~~

You can now use the GUI widget to add GUI elements to it
~~~~~~~~~~~~~{.cpp}
// Shows the text "Hello!" on the screen
GUIPanel* mainPanel = gui->getPanel();
mainPanel->addElement(GUILabel::create(HString("Hello!")));

// ... add more elements ...
~~~~~~~~~~~~~

Don't worry about what **GUIPanel** or **GUILabel** mean at this time, we'll talk about GUI panels, elements and layouts in later chapters. 

# Transforming GUI

Once you have set up a **GUIWidget** component, you can transform it using its scene object as normal. This allows you to apply 3D transformations to GUI elements, which can be useful for various interesting effects, including rendering GUI to in-game surfaces (like on a screen of an in-game 3D monitor).

~~~~~~~~~~~~~{.cpp}
// Rotate 30 degrees around the Z axis
Quaternion rotate(Vector3::UNIT_Z, Degree(30.0f));
guiSO->setRotation(rotate);
~~~~~~~~~~~~~

[TODO_IMAGE]()

# Using a separate GUI camera
In the example above we have asssumed you will use the same camera for GUI as you use for scene rendering. However sometimes it is useful to have a separate camera for GUI, or even multiple separate cameras. In such case camera creation is mostly the same, but with some additional options that need to be enabled. 

Initial creation of the camera is identical, we just choose a render target:

~~~~~~~~~~~~~{.cpp}
SPtr<RenderWindow> primaryWindow = gApplication().getPrimaryWindow();

HSceneObject guiCameraSO = SceneObject::create("GUI camera");
HCamera guiCamera = guiCameraSO->addComponent<CCamera>(primaryWindow);
~~~~~~~~~~~~~

In order to prevent the camera from rendering scene objects, we enable the **RenderSettings::overlayOnly** property on the camera's **RenderSettings** object.
~~~~~~~~~~~~~{.cpp}
auto rs = guiCamera->getRenderSettings();
rs->overlayOnly = true;

guiCamera->setRenderSettings(rs);
~~~~~~~~~~~~~

Now our camera will render just overlay objects (GUI and sprites), and nothing else. 

Next, we want to prevent the camera from clearing the render target. By default cameras will set all the pixels in the render target to some default value before they start rendering, every frame. We want our GUI camera to just render on top of anything rendered by the scene camera, so we disable that functionality by retrieving a @bs::Viewport from the camera. 

**Viewport** is retrieved by calling @bs::CCamera::getViewport. It allows you to set if the render target is cleared through @bs::Viewport::setClearFlags by providing the @bs::ClearFlagBits::Empty flag. 

~~~~~~~~~~~~~{.cpp}
SPtr<Viewport> viewport = guiCamera->getViewport();

// Disable clear for color, depth and stencil buffers
viewport->setClearFlags(ClearFlagBits::Empty);
~~~~~~~~~~~~~

You can also use the viewport to control onto which portion of the render target should the camera render to. By default it will output to the entire render target but you can change the area by calling @bs::Viewport::setArea.

At this point you can use the camera to create a **GUIWidget** and use the GUI as normal.
