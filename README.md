# KarvikDrive

A simple satellite imagery based driving game prototype, where you can control a 3D car model on an "infinite" world of ArcGIS data.
### Raylib 6.0 support has issues and doesn't work at the moment. This is because it seems newer versions of Raylib have issues with loading .jpg files disguised as .png files
### And because ArcGIS returns jpg files and the old fix was to simply tell Raylib to load them as .png data, now it's broken.
### To fix: enable JPG support in your build of Raylib 6, or use an older Raylib version that doesn't care about the actual magic bytes
