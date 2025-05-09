add_executable(game
    DeletionQueue.cpp
    VkInit.cpp
    VkUtil.cpp
    VkDescriptors.cpp
    VkPipelines.cpp
    VmaImpl.cpp

    util/GltfLoader.cpp
    util/InputUtil.cpp
    util/ImageLoader.cpp
    util/OSUtil.cpp

    Math/Transform.cpp
    Math/Util.cpp

    Graphics/Camera.cpp
    Graphics/FrustumCulling.cpp
    Graphics/Mesh.cpp
    Graphics/SkeletonAnimator.cpp
    Graphics/Texture.cpp


    MeshCache.cpp
    MaterialCache.cpp

    FreeCameraController.cpp

    Renderer.cpp
    Game.cpp

    main.cpp
)

set_target_properties(game PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_add_extra_warnings(game)

target_include_directories(game PRIVATE "${CMAKE_CURRENT_LIST_DIR}")

find_package(Vulkan REQUIRED)

target_link_libraries(game PRIVATE
    glfw::glfw

    Vulkan::Vulkan
    vk-bootstrap::vk-bootstrap

    GPUOpen::VulkanMemoryAllocator

        imgui::imgui

        tinygltf::tinygltf

        glm::glm

        stb::image
)

target_compile_definitions(game
        PUBLIC
            GLM_FORCE_CTOR_INIT
            GLM_FORCE_XYZW_ONLY
            GLM_FORCE_EXPLICIT_CTOR
            GLM_FORCE_DEPTH_ZERO_TO_ONE
            GLM_ENABLE_EXPERIMENTAL
)

set(SHADERS
    colored_triangle.vert
    colored_triangle.frag
    skinning.comp
    mesh.vert
    mesh.frag
    gradient.comp
)
list(TRANSFORM SHADERS PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/shaders/")

target_shaders(game ${SHADERS})

add_custom_command(TARGET game POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "Copying game assets to $<TARGET_FILE_DIR:game>/assets"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:game>/assets"
)
