# A Barebones 3D Game Engine/Renderer in C and SDL 3

This is a basic 3D rendering engine built using the C programming language and SDL3 library. It is single-threaded and purely CPU-bound, intended as an exploration and learning project rather than a production-ready product.

## Purpose and Motivation

I started this project for two main reasons:
1. To improve my understanding and proficiency in writing C code.
2. To challenge and apply my mathematics education in a practical context.

The project began with a very simple implementation and has gradually been optimized over time. However, there is still room for improvement, and I welcome suggestions and contributions from the community.

## Features and Capabilities

- Basic 3D rendering using C and SDL3
- Single-threaded, CPU-bound rendering
- Gradual optimizations for improved performance

## Installation and Setup

To run this project on your local machine, follow these steps:

1. Clone the repository: `git clone https://github.com/AlfieRan/BareBones-3D-Renderer.git`
2. Install the SDL3 library, which is the only non-standard dependency.
3. Compile the code using the provided Makefile. Please note that the Makefile is currently set up for a Macbook M3 Max (ARM chip), so it may need editing to compile on other systems.
4. Run the executable file generated after compilation.

The screen width and height can be changed in the `include/types.h` file, which also contains additional configuration settings.

## Contributing

If you have any suggestions or would like to contribute to this project, feel free to create a pull request (PR). I appreciate any feedback and contributions that can help improve the rendering engine.

## License

This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for more details.

## Contact

If you have any questions, suggestions, or feedback, please feel free to reach out to me:

- Email: hi@alfieranstead.com
- Twitter: [@alfieranstead](https://twitter.com/alfieranstead)
- GitHub: [AlfieRan](https://github.com/AlfieRan)

### Footnotes

Heavily Inspired by [JDH](https://github.com/jdah).

Useful Resources:

- [scratchapixel](https://www.scratchapixel.com/lessons/3d-basic-rendering/computing-pixel-coordinates-of-3d-point/mathematics-computing-2d-coordinates-of-3d-points.html)
- [3D Projection - Wikipedia](https://en.wikipedia.org/wiki/3D_projection#Perspective_projection)
- [Euler Angles - Wikipedia](https://en.wikipedia.org/wiki/Euler_angles#Table_of_matrices)
- [Triangle Rasterization](https://web.archive.org/web/20050408192410/http://sw-shader.sourceforge.net/rasterizer.html)
- [Circle Rasterization](https://en.wikipedia.org/wiki/Midpoint_circle_algorithm)
