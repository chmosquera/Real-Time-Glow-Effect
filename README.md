 # Real-time Glow Effect

Skills: C++, OpenGL

Topic: Computer graphics, Advanced rendering techniques, Real-time glow or "bloom"

![Video of glow](https://github.com/chmosquera/Real-Time-Glow-Effect/assets/25163297/00457b38-4c76-4a89-921e-3567108e7caf)

Video: Rotating Earth and moon, demonstrating the glow effect in different stages of the pipeline and with various glow intensity. 

## Pipeline

### 1. Render the scene into two textures
- **Color texture**: The standard color of the original image.
- **Glow mask texture**: A mask of the original image, indicating what pixels to enable glow.

![ColorTexture](https://github.com/chmosquera/Real-Time-Glow-Effect/assets/25163297/0fb8ef16-5c6d-48b1-8292-8bfc228b9bfc)

![GlowMaskTexture](https://github.com/chmosquera/Real-Time-Glow-Effect/assets/25163297/34bc038e-23eb-480e-ad90-15a0f14feb76)

### 2. Apply Gaussian blur to the glow mask
![GlowMaskBlurredTexture](https://github.com/chmosquera/Real-Time-Glow-Effect/assets/25163297/735283bf-4a5f-4419-b564-73e887c38550)

### 3. Blend the color and glow mask textures using additive blending
![Glow](https://github.com/chmosquera/Real-Time-Glow-Effect/assets/25163297/135aa718-07ad-48bd-880e-93bad9d2d1bd)

## Resources
- [LearnOpenGL: Bloom](https://learnopengl.com/Advanced-Lighting/Bloom)
- [GPU Gems: Real-Time Glow](GPU Gems: Real-Time Glow)
- [Unreal Engine: Bloom Post Process Effect](https://docs.unrealengine.com/en-US/Engine/Rendering/PostProcessEffects/Bloom/index.html)

## Acknowledgments
This project was created for the Advanced Rendering course at California Polytechnic State University, San Luis Obispo. 

Gratitude goes out to Professor Christian Eckhardt, my peers, and the computer graphics community for the wealth of knowledge and resources.

