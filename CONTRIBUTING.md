# Contributing to Spark Engine

Thank you for your interest in contributing to Spark Engine! This document provides guidelines and instructions for contributing.

## Getting Started

### Prerequisites

- **CMake** 3.20+
- **Visual Studio 2022** (Windows) or **GCC/Clang** (Linux/macOS)
- **Vulkan SDK** 1.3+
- **Git**

### Building from Source

1. Clone the repository:
```bash
git clone https://github.com/yourusername/SparkEngine.git
cd SparkEngine
```

2. Configure and build:
```bash
# Windows
cmake --preset windows-release
cmake --build --preset windows-release

# Linux
cmake --preset linux-release
cmake --build --preset linux-release
```

3. Run the editor:
```bash
# Windows
build/windows-release/editor/Release/SparkEditor.exe

# Linux
build/linux-release/editor/SparkEditor
```

## Development Workflow

### 1. Fork and Clone

1. Fork the repository on GitHub
2. Clone your fork locally
3. Add the upstream remote:
```bash
git remote add upstream https://github.com/yourusername/SparkEngine.git
```

### 2. Create a Branch

```bash
git checkout -b feature/your-feature-name
```

### 3. Make Changes

- Follow the coding style (see below)
- Write tests if applicable
- Update documentation

### 4. Commit and Push

```bash
git add .
git commit -m "Add your feature"
git push origin feature/your-feature-name
```

### 5. Create a Pull Request

1. Go to the original repository
2. Click "New Pull Request"
3. Select your branch
4. Fill in the PR template
5. Submit

## Coding Style

### C++ Guidelines

- Use **C++20** features
- Follow **snake_case** for functions and variables
- Use **PascalCase** for classes and structs
- Use **UPPER_SNAKE_CASE** for constants
- Add comments for complex logic
- Keep functions short and focused

### Example

```cpp
namespace spark {

class MyComponent : public Component {
public:
    float myValue = 0.0f;
    bool isActive = true;

    void update(float deltaTime) {
        if (isActive) {
            myValue += deltaTime;
        }
    }
};

} // namespace spark
```

### File Organization

- **Headers**: `*.h` in `engine/`, `editor/`, etc.
- **Implementation**: `*.cpp` alongside headers
- **Shaders**: `*.glsl` in `assets/shaders/`
- **Examples**: In `examples/` directory

## Reporting Issues

### Bug Reports

When reporting bugs, please include:
1. Steps to reproduce
2. Expected behavior
3. Actual behavior
4. System information (OS, GPU, etc.)
5. Screenshots if applicable

### Feature Requests

When requesting features, please include:
1. Description of the feature
2. Use case
3. Proposed implementation (if any)

## Code of Conduct

- Be respectful
- Be constructive
- Be patient
- Be helpful

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

## Questions?

If you have questions, feel free to:
- Open an issue
- Join our Discord (if available)
- Email the maintainers

Thank you for contributing to Spark Engine! 🚀
