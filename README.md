### Code: [![OpenSSF Scorecard](https://api.scorecard.dev/projects/github.com/LiangRVR/Ray-tracing-vulkan/badge)](https://api.scorecard.dev/projects/github.com/LiangRVR/Ray-tracing-vulkan) [![OpenSSF Best Practices](https://www.bestpractices.dev/projects/10242/badge)](https://www.bestpractices.dev/projects/10242)

## About This Project
This project is a Vulkan-based ray tracing application built on top of the Walnut framework. It leverages Walnut's integration with Dear ImGui for creating a user-friendly interface and provides a foundation for experimenting with real-time ray tracing techniques. The goal is to demonstrate the capabilities of Vulkan for high-performance rendering and to serve as a learning resource for developers interested in graphics programming and ray tracing.

# Walnut

Walnut [(base project)](https://github.com/StudioCherno/Walnut/tree/master)  is a simple application framework built with Dear ImGui and designed to be used with Vulkan - basically this means you can seemlessly blend real-time Vulkan rendering with a great UI library to build desktop applications. The plan is to expand Walnut to include common utilities to make immediate-mode desktop apps and simple Vulkan applications.

## Requirements
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) (preferably a recent version)

## Getting Started
Once you've cloned, run `scripts/Setup.bat` to generate Visual Studio 2022 solution/project files. Once you've opened the solution, you can run the WalnutApp project to see a basic example (code in `WalnutApp.cpp`). I recommend modifying that WalnutApp project to create your own application, as everything should be setup and ready to go.

### 3rd party libaries
- [Dear ImGui](https://github.com/ocornut/imgui)
- [GLFW](https://github.com/glfw/glfw)
- [stb_image](https://github.com/nothings/stb)
- [GLM](https://github.com/g-truc/glm) (included for convenience)

## Contributing and Feedback

### Obtaining the Software
To obtain the software, clone the repository using the following command:
```bash
git clone https://github.com/LiangRVR/Ray-tracing-vulkan.git
```

### Providing Feedback
We welcome feedback in the form of bug reports or enhancement suggestions. To provide feedback:
1. Navigate to the [Issues](https://github.com/LiangRVR/Ray-tracing-vulkan/issues) section of the repository.
2. Create a new issue, providing as much detail as possible, including steps to reproduce bugs or a clear description of the enhancement.

### Contributing
Contributions are highly appreciated! To contribute:
1. Fork the repository.
2. Create a new branch for your feature or bug fix:
    ```bash
    git checkout -b feature-or-bugfix-name
    ```
3. Make your changes and commit them with clear messages.
4. Push your branch to your fork:
    ```bash
    git push origin feature-or-bugfix-name
    ```
5. Open a pull request to the main repository, describing your changes in detail.

Please ensure your contributions adhere to the project's coding standards and include appropriate documentation or tests where applicable.

### Submitting Bug Reports

To submit a bug report, follow these steps:

1. **Check for Existing Issues**: Before creating a new bug report, search the [Issues](https://github.com/LiangRVR/Ray-tracing-vulkan/issues) section to see if the issue has already been reported.

2. **Create a New Issue**:
    - Go to the [Issues](https://github.com/LiangRVR/Ray-tracing-vulkan/issues) page.
    - Click on the "New Issue" button.

3. **Provide Detailed Information**:
    - **Title**: Use a clear and descriptive title for the issue.
    - **Description**: Include a detailed description of the bug, including:
      - Steps to reproduce the issue.
      - Expected behavior.
      - Actual behavior.
    - **Environment**: Specify the environment where the issue occurred, such as:
      - Operating system and version.
      - Vulkan SDK version.
      - Any other relevant details.

4. **Attach Logs or Screenshots** (if applicable): If possible, include logs, error messages, or screenshots to help diagnose the issue.

5. **Submit the Issue**: Once all the information is provided, click "Submit new issue."

By following these steps, you help ensure that the bug can be addressed efficiently. Thank you for contributing to the project!

### Participating in Discussions
1. **Join Existing Discussions**: Check the [Discussions](https://github.com/LiangRVR/Ray-tracing-vulkan/discussions) section to participate in ongoing conversations.
2. **Start a New Discussion**:
    - Go to the [Discussions](https://github.com/LiangRVR/Ray-tracing-vulkan/discussions) page.
    - Click on "New Discussion" and choose the appropriate category (e.g., Q&A, Ideas, etc.).
    - Share your thoughts, questions, or suggestions with the community.

By reporting issues and engaging in discussions, you contribute to the project's growth and help foster a collaborative environment. Thank you for your support!

### Additional
- Walnut uses the [Roboto](https://fonts.google.com/specimen/Roboto) font ([Apache License, Version 2.0](https://www.apache.org/licenses/LICENSE-2.0))
