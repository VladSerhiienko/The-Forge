cmake_minimum_required(VERSION 3.4.1)

include(ExternalProject)
project(TheForge)

message(STATUS "CMAKE_SYSTEM = ${CMAKE_SYSTEM}")
message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_INFO_FILE = ${CMAKE_SYSTEM_INFO_FILE}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR = ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
message(STATUS "VK_SDK_PATH = $ENV{VK_SDK_PATH}")

set(BUILD_FOLDER_SUFFIX "build_${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}")
string(TOLOWER  ${BUILD_FOLDER_SUFFIX} BUILD_FOLDER_SUFFIX)
message(STATUS "BUILD_FOLDER_SUFFIX = ${BUILD_FOLDER_SUFFIX}")

if (WIN32)
    set(
        default_cmake_args
        -G
        "Visual Studio 15 2017 Win64"
        -B${BUILD_FOLDER_SUFFIX} "-H."
    )
else()
    set(
        default_cmake_args
        -B${BUILD_FOLDER_SUFFIX} -H.
    )
endif()

ExternalProject_Add(
    flatbuffers
    GIT_REPOSITORY "git@github.com:google/flatbuffers.git"
    GIT_TAG "master"
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/flatbuffers"
    CMAKE_ARGS ${default_cmake_args}
    TEST_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(flatbuffers SOURCE_DIR)
ExternalProject_Get_Property(flatbuffers BINARY_DIR)
set(flatbuffers_source_dir ${SOURCE_DIR})
set(flatbuffers_binary_dir ${BINARY_DIR})
message(STATUS "flatbuffers_source_dir = ${flatbuffers_source_dir}")
message(STATUS "flatbuffers_binary_dir = ${flatbuffers_binary_dir}")

ExternalProject_Add(
    FbxPipeline
    GIT_REPOSITORY "git@github.com:VladSerhiienko/FbxPipeline.git"
    GIT_TAG "master"
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/FbxPipeline"
    CMAKE_ARGS ${default_cmake_args}
    TEST_COMMAND ""
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(FbxPipeline SOURCE_DIR)
ExternalProject_Get_Property(FbxPipeline BINARY_DIR)
set(FbxPipeline_source_dir ${SOURCE_DIR})
set(FbxPipeline_binary_dir ${BINARY_DIR})
message(STATUS "FbxPipeline_source_dir = ${FbxPipeline_source_dir}")
message(STATUS "FbxPipeline_binary_dir = ${FbxPipeline_binary_dir}")

ExternalProject_Add(
    spdlog
    GIT_REPOSITORY "git@github.com:gabime/spdlog.git"
    GIT_TAG "master"
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/spdlog"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    LOG_DOWNLOAD ON
)

ExternalProject_Get_Property(spdlog SOURCE_DIR)
set(spdlog_source_dir ${SOURCE_DIR})
message(STATUS "spdlog_source_dir = ${spdlog_source_dir}")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

if (WIN32)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_CRT_SECURE_NO_WARNINGS -DUNICODE -D_UNICODE")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_CRT_SECURE_NO_WARNINGS -DUNICODE -D_UNICODE")
endif()

#
#
# Renderer (Direct3D12)
#
#

if (WIN32)
    add_library(
        RendererDX12  
        STATIC
        ${CMAKE_SOURCE_DIR}/Common_3/Renderer/Direct3D12/Direct3D12.cpp
        ${CMAKE_SOURCE_DIR}/Common_3/Renderer/Direct3D12/Direct3D12Hooks.cpp
        ${CMAKE_SOURCE_DIR}/Common_3/Renderer/Direct3D12/Direct3D12Hooks.h
        ${CMAKE_SOURCE_DIR}/Common_3/Renderer/Direct3D12/Direct3D12MemoryAllocator.cpp
        ${CMAKE_SOURCE_DIR}/Common_3/Renderer/Direct3D12/Direct3D12MemoryAllocator.h
        ${CMAKE_SOURCE_DIR}/Common_3/Renderer/Direct3D12/Direct3D12ShaderReflection.cpp
        ${CMAKE_SOURCE_DIR}/Common_3/Renderer/CommonShaderReflection.cpp
        ${CMAKE_SOURCE_DIR}/Common_3/Renderer/GpuProfiler.cpp
        ${CMAKE_SOURCE_DIR}/Common_3/Renderer/GpuProfiler.h
        ${CMAKE_SOURCE_DIR}/Common_3/Renderer/IMemoryAllocator.h
        ${CMAKE_SOURCE_DIR}/Common_3/Renderer/IRenderer.h
        ${CMAKE_SOURCE_DIR}/Common_3/Renderer/IShaderReflection.h
        ${CMAKE_SOURCE_DIR}/Common_3/Renderer/ResourceLoader.cpp
        ${CMAKE_SOURCE_DIR}/Common_3/Renderer/ResourceLoader.h
    )

    target_include_directories(
        RendererDX12
        PUBLIC
        ${CMAKE_SOURCE_DIR}/Common_3
    )

    target_compile_definitions(
        RendererDX12
        PRIVATE
        DIRECT3D12=1
    )

    set_target_properties(
        RendererDX12
        PROPERTIES
        FOLDER
        Libraries/Direct3D12
    )
endif()

#
#
# Renderer (Vulkan)
#
#

add_library(
    RendererVk  
    STATIC
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/VulkanMemoryAllocator/VulkanMemoryAllocator.h
    ${CMAKE_SOURCE_DIR}/Common_3/Renderer/Vulkan/Vulkan.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/Renderer/Vulkan/VulkanShaderReflection.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/Renderer/CommonShaderReflection.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/Renderer/GpuProfiler.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/Renderer/GpuProfiler.h
    ${CMAKE_SOURCE_DIR}/Common_3/Renderer/IMemoryAllocator.h
    ${CMAKE_SOURCE_DIR}/Common_3/Renderer/IRenderer.h
    ${CMAKE_SOURCE_DIR}/Common_3/Renderer/IShaderReflection.h
    ${CMAKE_SOURCE_DIR}/Common_3/Renderer/ResourceLoader.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/Renderer/ResourceLoader.h
)

target_include_directories(
    RendererVk
    PUBLIC
    ${CMAKE_SOURCE_DIR}/Common_3
    $ENV{VK_SDK_PATH}/include
)

target_compile_definitions(
    RendererVk
    PRIVATE
    VULKAN=1
    USE_MEMORY_TRACKING=1
)

set_target_properties(
    RendererVk
    PROPERTIES
    FOLDER
    Libraries/Vulkan
)

#
#
# OS
#
#

set(
    OS_source_files
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Camera/FpsCameraController.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Camera/GuiCameraController.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Core/Compiler.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Core/DLL.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Core/FileSystem.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Core/PlatformEvents.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Core/RingBuffer.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Core/ThreadSystem.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Core/Timer.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Image/Image.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Image/Image.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Image/ImageEnums.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Image/ImageKTXImpl.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Interfaces/IApp.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Interfaces/ICameraController.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Interfaces/IFileSystem.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Interfaces/ILogManager.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Interfaces/IMemoryManager.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Interfaces/IOperatingSystem.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Interfaces/IPlatformEvents.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Interfaces/IThread.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Interfaces/ITimeManager.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Interfaces/IUIManager.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Logging/LogManager.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Logging/LogManager.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Math/float2.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Math/float3.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Math/float4.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Math/FloatUtil.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Math/FloatUtil.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Math/half.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Math/half.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Math/IntersectionHelpers.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Math/IntersectionHelpers.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Math/mat2.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Math/mat2.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Math/mat3.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Math/mat4.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Math/MathTypes.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Math/Noise.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Math/Noise.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Math/vmInclude.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/MemoryTracking/malloc-2.8.6.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/MemoryTracking/MemoryTrackingManager.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/UI/Fontstash.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/UI/Fontstash.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/UI/NuklearGUIDriver.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/UI/NuklearGUIDriver.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/UI/UI.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/UI/UI.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/UI/UIManager.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/UI/UIRenderer.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/UI/UIRenderer.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/UI/UIShaders.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Windows/mainicon.ico
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Windows/resource.h
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Windows/Resources.aps
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Windows/Resources.rc
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Windows/WindowsBase.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Windows/WindowsFileSystem.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Windows/WindowsLogManager.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/OS/Windows/WindowsThreadManager.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/TinyEXR/tinyexr.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/FluidStudios/MemoryManager/mmgr.cpp
)

add_library(
    OSVk
    STATIC
    ${OS_source_files}
)

target_include_directories(
    OSVk
    PUBLIC
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/FluidStudios/MemoryManager
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/TinyEXR
    ${CMAKE_SOURCE_DIR}/Common_3
    $ENV{VK_SDK_PATH}/include
)

target_compile_definitions(
    OSVk
    PRIVATE
    VULKAN=1
    USE_MEMORY_TRACKING=1    
)

set_target_properties(
    OSVk
    PROPERTIES
    FOLDER
    Libraries/Vulkan
)

if (WIN32)

    add_library(
        OSDX12
        STATIC
        ${OS_source_files}
    )

    target_include_directories(
        OSDX12
        PUBLIC
        ${CMAKE_SOURCE_DIR}/Common_3
        $ENV{VK_SDK_PATH}/include
    )

    target_compile_definitions(
        OSDX12
        PRIVATE
        DIRECT3D12=1
    )

    set_target_properties(
        OSDX12
        PROPERTIES
        FOLDER
        Libraries/Direct3D12
    )

endif()

#
#
# SpirvTools
#
#

add_library(
    SpirvTools
    SHARED
    ${CMAKE_SOURCE_DIR}/Common_3/Tools/SpirvTools/dllmain.cpp    
    ${CMAKE_SOURCE_DIR}/Common_3/Tools/SpirvTools/SpirvTools.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/Tools/SpirvTools/SpirvTools.h
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/SPIRV_Cross/spirv.hpp
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/SPIRV_Cross/spirv_cfg.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/SPIRV_Cross/spirv_cfg.hpp
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/SPIRV_Cross/spirv_common.hpp
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/SPIRV_Cross/spirv_cross.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/SPIRV_Cross/spirv_cross.hpp
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/SPIRV_Cross/spirv_cpp.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/SPIRV_Cross/spirv_cpp.hpp
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/SPIRV_Cross/spirv_glsl.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/SPIRV_Cross/spirv_glsl.hpp
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/SPIRV_Cross/spirv_hlsl.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/SPIRV_Cross/spirv_hlsl.hpp
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/SPIRV_Cross/spirv_msl.cpp
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/SPIRV_Cross/spirv_msl.hpp
)

target_compile_definitions(
    SpirvTools
    PRIVATE
    API_EXPORT=1
)

set_target_properties(
    SpirvTools
    PROPERTIES
    FOLDER
    Libraries/Vulkan
)

#
#
# TransformationsVk
#
#

file(
    GLOB 
    TransformationsVk_shader_source_files
    "${CMAKE_SOURCE_DIR}/Examples_3/Unit_Tests/src/01_Transformations/PCVulkan/*.vert"
    "${CMAKE_SOURCE_DIR}/Examples_3/Unit_Tests/src/01_Transformations/PCVulkan/*.frag"
    "${CMAKE_SOURCE_DIR}/Examples_3/Unit_Tests/src/01_Transformations/PCVulkan/*.tesc"
    "${CMAKE_SOURCE_DIR}/Examples_3/Unit_Tests/src/01_Transformations/PCVulkan/*.tese"
    "${CMAKE_SOURCE_DIR}/Examples_3/Unit_Tests/src/01_Transformations/PCVulkan/*.comp"
    "${CMAKE_SOURCE_DIR}/Examples_3/Unit_Tests/src/01_Transformations/PCVulkan/*.geom"
)

add_executable(
    TransformationsVk
    ${CMAKE_SOURCE_DIR}/Examples_3/Unit_Tests/src/01_Transformations/01_Transformations.cpp
    ${TransformationsVk_shader_source_files}
)

source_group("Shader Files" FILES ${TransformationsVk_shader_source_files})

make_directory( "${CMAKE_SOURCE_DIR}/Examples_3/Unit_Tests/src/01_Transformations/PCVulkan/Binary/" )
make_directory( "${CMAKE_BINARY_DIR}${CONFIGURATION_SUFFIX}/Transformations/" )
make_directory( "${CMAKE_BINARY_DIR}${CONFIGURATION_SUFFIX}/Transformations/PCVulkan/" )
make_directory( "${CMAKE_BINARY_DIR}${CONFIGURATION_SUFFIX}/Transformations/PCVulkan/Binary/" )

foreach( TransformationsVk_shader_source_file ${TransformationsVk_shader_source_files} )
    message(STATUS "Adding ${TransformationsVk_shader_source_file} for glslangValidator build")
    add_custom_command(
        TARGET
        TransformationsVk
        PRE_BUILD
        COMMAND
            $ENV{VK_SDK_PATH}/bin/glslangValidator.exe
            -V "${TransformationsVk_shader_source_file}"
            -o "${TransformationsVk_shader_source_file}.spv"
        COMMAND ${CMAKE_COMMAND}
            -E copy_if_different
            "${TransformationsVk_shader_source_file}"
            "${CMAKE_BINARY_DIR}${CONFIGURATION_SUFFIX}/Transformations/PCVulkan/"
        COMMAND ${CMAKE_COMMAND}
            -E copy_if_different
            "${TransformationsVk_shader_source_file}.spv"
            "${CMAKE_BINARY_DIR}${CONFIGURATION_SUFFIX}/Transformations/PCVulkan/Binary/"
    )
endforeach()

target_include_directories(
    TransformationsVk
    PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Common_3
    $ENV{VK_SDK_PATH}/include
)

target_link_libraries(
    TransformationsVk
    OSVk
    RendererVk
    SpirvTools
    $ENV{VK_SDK_PATH}/lib/vulkan-1.lib
)

target_compile_definitions(
    TransformationsVk
    PRIVATE
    VULKAN=1
    USE_MEMORY_TRACKING=1    
)

set_target_properties(
    TransformationsVk
    PROPERTIES
    FOLDER
    UnitTests/Vulkan
    VS_DEBUGGER_WORKING_DIRECTORY
    "$(OutDir)"
)

#
#
# SceneViewer
#
#

file(
    GLOB 
    SceneViewerVk_shader_source_files
    "${CMAKE_SOURCE_DIR}/Examples_3/SceneViewer/src/PCVulkan/*.vert"
    "${CMAKE_SOURCE_DIR}/Examples_3/SceneViewer/src/PCVulkan/*.frag"
    "${CMAKE_SOURCE_DIR}/Examples_3/SceneViewer/src/PCVulkan/*.tesc"
    "${CMAKE_SOURCE_DIR}/Examples_3/SceneViewer/src/PCVulkan/*.tese"
    "${CMAKE_SOURCE_DIR}/Examples_3/SceneViewer/src/PCVulkan/*.comp"
    "${CMAKE_SOURCE_DIR}/Examples_3/SceneViewer/src/PCVulkan/*.geom"
)

add_executable(
    SceneViewerVk
    ${CMAKE_SOURCE_DIR}/Examples_3/SceneViewer/src/Scene.h
    ${CMAKE_SOURCE_DIR}/Examples_3/SceneViewer/src/Scene.cpp
    # ${CMAKE_SOURCE_DIR}/Examples_3/SceneViewer/src/Geometry.h
    # ${CMAKE_SOURCE_DIR}/Examples_3/SceneViewer/src/Geometry.cpp
    ${CMAKE_SOURCE_DIR}/Examples_3/SceneViewer/src/SceneViewer.cpp
    ${SceneViewerVk_shader_source_files}
)

add_dependencies(
    SceneViewerVk
    spdlog
    flatbuffers
    FbxPipeline
)

source_group("Shader Files" FILES ${SceneViewerVk_shader_source_files})

make_directory( "${CMAKE_BINARY_DIR}${CONFIGURATION_SUFFIX}/SceneViewer/" )
make_directory( "${CMAKE_BINARY_DIR}${CONFIGURATION_SUFFIX}/SceneViewer/PCVulkan/" )
make_directory( "${CMAKE_BINARY_DIR}${CONFIGURATION_SUFFIX}/SceneViewer/PCVulkan/Binary/" )

foreach( SceneViewerVk_shader_source_file ${SceneViewerVk_shader_source_files} )
    message(STATUS "Adding ${SceneViewerVk_shader_source_file} for glslangValidator build")
    add_custom_command(
        TARGET
        SceneViewerVk
        DEPENDS "${SceneViewerVk_shader_source_file}"
        POST_BUILD
        COMMAND
            $ENV{VK_SDK_PATH}/bin/glslangValidator.exe
            "${CMAKE_SOURCE_DIR}/Examples_3/SceneViewer/src/PCVulkan/config.conf"
            -V "${SceneViewerVk_shader_source_file}"
            -o "${SceneViewerVk_shader_source_file}.spv"
        COMMAND ${CMAKE_COMMAND}
            -E copy_if_different
            "${SceneViewerVk_shader_source_file}"
            "${CMAKE_BINARY_DIR}${CONFIGURATION_SUFFIX}/SceneViewer/PCVulkan/"
        COMMAND ${CMAKE_COMMAND}
            -E copy_if_different
            "${SceneViewerVk_shader_source_file}.spv"
            "${CMAKE_BINARY_DIR}${CONFIGURATION_SUFFIX}/SceneViewer/PCVulkan/Binary/"
    )
endforeach()

target_include_directories(
    SceneViewerVk
    PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Common_3/ThirdParty/OpenSource/FbxPipeline
    ${CMAKE_SOURCE_DIR}/Common_3
    ${flatbuffers_source_dir}/include
    ${flatbuffers_source_dir}/grpc
    $ENV{VK_SDK_PATH}/include
)

target_link_libraries(
    SceneViewerVk
    OSVk
    RendererVk
    SpirvTools
    
    debug ${flatbuffers_binary_dir}/Debug/flatbuffers.lib
    optimized ${flatbuffers_binary_dir}/Release/flatbuffers.lib
    # ${flatbuffers_binary_dir}/libflatbuffers.a

    $ENV{VK_SDK_PATH}/lib/vulkan-1.lib
)

target_compile_definitions(
    SceneViewerVk
    PRIVATE
    VULKAN=1
    USE_MEMORY_TRACKING=1    
)

set_target_properties(
    SceneViewerVk
    PROPERTIES
    FOLDER
    Projects/Vulkan
    VS_DEBUGGER_WORKING_DIRECTORY
    "$(OutDir)"
)

#
#
# Finalization
#
#

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PREDEFINED_TARGETS_FOLDER "CustomTargets")