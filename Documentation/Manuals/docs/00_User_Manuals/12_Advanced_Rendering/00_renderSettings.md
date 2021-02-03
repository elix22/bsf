---
title: Render settings
---

@bs::RenderSettings is an object present on every **Camera** object. It can be retrieved through @bs::CCamera::getRenderSettings() and allows you to customize what rendering effects are executed when rendering the scene through that view. 

For complete list of tweakable properties check the API reference, and here we'll just cover the main points.

Note that after you change any of the properties in **RenderSettings** you must call @bs::CCamera::setRenderSettings() to apply the changes to the camera.

~~~~~~~~~~~~~{.cpp}
HCamera camera = ...;

// Tweak the render settings by disabling some effects
auto rs = camera->getRenderSettings();
rs->screenSpaceReflections.enabled = false;
rs->ambientOcclusion.enabled = false;
rs->enableIndirectLighting = false;
rs->enableFXAA = false;

camera->setRenderSettings(rs);
~~~~~~~~~~~~~

# HDR and tonemapping
HDR stands for high-dynamic range, and it allows the lights in the scene to use a large range of intensity values that can more closely approximate a real-world scene. Lighting information is first written to a floating point texture that can store a wider range of values than a normal RGB texture. These high range lighting values are then used throughout the calculations in the engine, ensuring a higher quality final result. You can toggle HDR rendering through @bs::RenderSettings::enableHDR.

Before the image is output to the screen it goes through the process called tonemapping, which converts the high range values into low range that a normal output device (like a monitor or a TV) can display. Tonemapping can be toggled through @bs::RenderSettings::enableTonemapping, but this is generally something you always want to have on when HDR rendering is enabled.

@bs::RenderSettings::tonemapping provides you with a variety of tweakable options on how is tonemapping performed. Tonemapping operators are a complex topic, but it's enough to know that these options affect the feel and tone of the final image, and it is generally something you will tweak to suit the best look for your project.

HDR and tonemapping is also closely related to exposure, which we'll cover next.

# Exposure
Exposure determines which part of the high range image should be converted to low range (e.g. the very bright parts, the very dark parts, or somewhere in the middle). Generally this is a property you will only use when HDR is enabled, as LDR doesn't offer a high enough range for this property to be relevant.

By default the system will calculate the exposure automatically, based on how the human eye determines exposure. Generally this means if you are in a very bright area, it will be hard to see into darker areas (imagine standing outside in sunlight and looking into house lit only by artifical light), or when in a very dark area the bright areas will be overexposed. As you move between areas of different light intensity the exposure will slowly adjust accordingly. You can tweak automatic exposure options through @bs::RenderSettings::autoExposure (similar to human eyes - imagine walking into a house on a brightly Sun-lit day, it takes a while for your eyes to adjust).

Automatic exposure can be disabled through @bs::RenderSettings::enableAutoExposure. In this case you will want to set the exposure manually through @bs::RenderSettings::exposureScale. This allows for more control over the exposure, which is sometimes required.

[TODO_IMAGE]()
Image with different exposure levels

# White balance
White balance is a process that occurs during tonemapping and therefore requires tonemapping to be enabled. It is intended to emulate the effect of human vision called 'chromatic adaptation', where our eyes are able to adjust to different lighting conditions while still being able to tell actual colors of a surface (e.g. a blue car illuminated by a strong red light still looks blue). 

When it comes to virtual lighting our eyes cannot perform the same adapation, as they will adjust to the real-world environment instead of the in-game environment (e.g. to the room where your screen is) . The white balance process converts in-game lighting to some real-world lighting, ensuring this adjustment process is emulated as if you were in the in-game lighting environment. 

By default the real-world lighting is assumed to be a room lit by daylight, but the exact environment can be controlled through @bs::RenderSettings::whiteBalance. Since you cannot assume the lighting environment your application will be viewed under, this might be best left for the user to tweak.

[TODO_IMAGE]()
Image with different white balance lighting environments

# Color grading
Color grading allows you to perform additional artistic control over the final image and tweak settings like contrast and saturation. These effects are not physically based in nature and can be tweaked for purely artistic control.

The relevant options are present in @bs::RenderSettings::colorGrading.

[TODO_IMAGE]()
Image with and without custom color grading

# Screen space reflections (SSR)
This effect provides high quality, real-time reflections at a fairly low performance impact. The main limitation effect is that it is performed in screen-space, and therefore cannot reflect an object that's not currently on the screen. When reflection cannot be found the system will fall back onto reflection probes. The effect is also generally not suitable for perfect mirror-like reflections due to limited precision.

You can control and toggle the effect through @bs::RenderSettings::screenSpaceReflections.

[TODO_IMAGE]()
Left - SSR disabled, Right - SSR enabled

# Screen space ambient occlusion (SSAO)
This effect estimates ambient occlusion using screen-space information. The ambient occlusion is approximated by sampling the nearby geometry and determining the occlusion amount. More nearby geometry results in a higher occlusion value, and therefore the surface receiving less light. This produces more realistic lighting. Note that SSAO is by default applied only to indirect lighting, and as such it is mostly visible in shadows.

You can control and toggle the effect through @bs::RenderSettings::ambientOcclusion.

[TODO_IMAGE]()
Left - SSAO disabled, Right - SSAO enabled

# Depth of field
By default the virtual camera focuses perfectly on all parts of the scene it views, but this is not the case with real-world cameras. Real-world cameras can instead set a focus distance at which the captured image will be perfectly in focus. Anything closer or further away from that distance will get progressively more out of focus (blurry). Depth of field effect emulates this functionality of the camera, allowing you to set a focus distance while blurring the near and/or far parts of the scene.

Depth of field options can be tweaked through @bs::RenderSettings::depthOfField.

[TODO_IMAGE]()
Left - DOF disabled, Right - DOF enabled

# Fast approximate anti-aliasing (FXAA)
This is a screen space effect that reduces the aliasing artifacts known as 'jaggies'. These artifacts occur when there are discontinuities while rendering a pixel, for example when a pixel contains an edge between two surfaces. This effect is an alternative to other anti-aliasing methods like MSAA. It is significantly faster than MSAA but can result in lower quality and greater overall bluriness of the resulting image.

It can be toggled through @bs::RenderSettings::enableFXAA.

[TODO_IMAGE]()
Image without FXAA

[TODO_IMAGE]()
Image with FXAA

# Gamma
Tweaks the gamma value that's applied to the image before being sent to the output device. Mainly affects the brightness of the image.

Controlled through @bs::RenderSettings::gamma.

# Shadows
Shadow rendering for a specific view can be completely disabled through @bs::RenderSettings::enableShadows. Shadow options that are view-specific can be controlled through @bs::RenderSettings::shadowSettings.

# Indirect lighting
Indirect lighting provided by **LightProbeVolume** can be fully disabled for a view through @bs::RenderSettings::enableIndirectLighting.

# Bloom
Bloom provides an extra highlight to already bright areas of the scene, simulating the real-world camera effect where an extremely bright light overwhelms the camera. The effect produces a fringe extending from the borders of the bright areas of the image. This effect is only used if tonemapping and HDR is enabled.

![Image without bloom](../../Images/BloomOffSmall.png)  
![Image with bloom](../../Images/BloomOnSmall.png)  

You can enable/disable, as well as tweak the effect through @bs::RenderSettings::bloom. You can control the intensity treshold at which bloom will be shown, as well as tweak the global intensity and tint, along with a quality setting for performance/quality tradeoff.
