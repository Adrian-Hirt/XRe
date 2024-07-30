# XRe

XRe is a project to build a XR engine from scratch using OpenXR and DirectX.

_Please note that XRe is a WIP project and mainly intended for me to learn more about OpenXR, DirectX and XR development for the moment. It is
not (yet) meant to be used for XR development directly._

## Features

XRe currently has the following features:

* Full Rendering loop with separate methods to update the simulation (with predicted render time passed in) and render the scene.
* Defining models from vertices as well as loading models from `.obj` files
* Rendering text and flat bitmaps
* Rendering simple lines
* Controller tracking
  - Tracking of the `grab` action on controllers
* Ability to move / rotate objects in the scene by grabbing
* Bounding boxes for objects
* Teleporting to other locations in the scene
* Tracking the aim of the user and highlighting intersections of the aim and the scene.
* Various shaders (e.g. ambient lighting, textured models)
* Basic hand tracking

### Screenshots

Simple scene:

![](doc/img/screenshot_mr_portal_1.png)

Hand tracking:

![](doc/img/hand_tracking.jpeg)

## Acknowledgements

* The textures in `data/textures` were downloaded from https://ambientcg.com/, licensed under the Creative Commons CC0 1.0 Universal License.
* XRe is using [DirectXMath](https://github.com/microsoft/DirectXMath), February 2024 version, licensed under the MIT license.
* XRe is using [DirectXTex](https://github.com/microsoft/DirectXTex), licensed under the MIT license.
