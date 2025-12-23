cmake_minimum_required(VERSION 3.16)

project(Pix3lForge VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC specific configuration for Qt6
if(MSVC)
    add_compile_options(/Zc:__cplusplus /permissive-)
endif()

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Concurrent Svg Network)

# Set project files
set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/imageprocessor.h
    src/imageprocessor.cpp
    src/model/imagedocument.h
    src/model/imagedocument.cpp
    src/model/adjustmentparameters.h
    src/widgets/propertiespanel.h
    src/widgets/propertiespanel.cpp
    src/commands/imagecommand.h
    src/commands/imagecommand.cpp
    src/commands/commandmanager.h
    src/commands/commandmanager.cpp
    src/commands/commandfactory.h
    src/commands/commandfactory.cpp
    src/view/viewmanager.h
    src/view/viewmanager.cpp
    src/dialogs/dialogmanager.h
    src/dialogs/dialogmanager.cpp
    src/dialogs/aboutdialog.h
    src/dialogs/aboutdialog.cpp
    src/dialogs/logviewerdialog.h
    src/dialogs/logviewerdialog.cpp
    src/preview/previewmanager.h
    src/preview/previewmanager.cpp
    src/actions/actionmanager.h
    src/actions/actionmanager.cpp
    src/settings/settingsmanager.h
    src/settings/settingsmanager.cpp
    src/logging/logger.h
    src/logging/logger.cpp
    src/pix3ltheme.h
    src/pix3ltheme.cpp
    # AI Enhancement
    src/ai/IAIProvider.h
    src/ai/EnhancementResponseParser.h
    src/ai/EnhancementResponseParser.cpp
    src/ai/ImageEncoder.h
    src/ai/ImageEncoder.cpp
    src/ai/EnhancementPromptBuilder.h
    src/ai/EnhancementPromptBuilder.cpp
    src/ai/RetryPolicy.h
    src/ai/AIProviderFactory.h
    src/ai/AIProviderFactory.cpp
    src/ai/providers/LMStudioProvider.h
    src/ai/providers/LMStudioProvider.cpp
    src/ai/providers/OpenRouterProvider.h
    src/ai/providers/OpenRouterProvider.cpp
    src/ai/providers/OpenAIProvider.h
    src/ai/providers/OpenAIProvider.cpp
    src/ai/providers/AnthropicProvider.h
    src/ai/providers/AnthropicProvider.cpp
    src/dialogs/AISettingsDialog.h
    src/dialogs/AISettingsDialog.cpp
    src/dialogs/AIEnhancementDialog.h
    src/dialogs/AIEnhancementDialog.cpp
)

# Windows icon resource
if(WIN32)
    set(APP_ICON_RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/resources/Pix3lForge.rc)
endif()

# Qt resources
set(PROJECT_RESOURCES
    resources/resources.qrc
)

# Enable AUTOMOC for Qt6
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Create executable
if(WIN32)
    qt6_add_executable(Pix3lForge ${PROJECT_SOURCES} ${PROJECT_RESOURCES} ${APP_ICON_RESOURCE})
else()
    qt6_add_executable(Pix3lForge ${PROJECT_SOURCES} ${PROJECT_RESOURCES})
endif()

# Link Qt libraries
target_link_libraries(Pix3lForge PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::Concurrent
    Qt6::Svg
    Qt6::Network
)

# Set target properties
set_target_properties(Pix3lForge PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)