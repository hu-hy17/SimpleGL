#--- Collect files
FILE(GLOB SOURCE_FILES "*.cpp" "*.h")
FILE(GLOB SHADER_FILES "resources/shader/*.vert" "resources/shader/*.frag")
FILE(GLOB UI_FILES "*.ui")
FILE(GLOB QRC_FILE "SimpleGL.qrc")

#--- Copy resources files not managed with qrc
FILE(COPY "resources/scene" "resources/model" "resources/skybox" DESTINATION ${PROJECT_BINARY_DIR}/SimpleGL/resources)

#--- Collect scene settings
FILE(GLOB SCENE_SETTINGS ${PROJECT_BINARY_DIR}/SimpleGL/resources/scene/"resources/scene/*.json")

#--- Sub group division
source_group(Resources FILES ${UI_FILES} ${QRC_FILE})
source_group(Resources/Config FILES ${SCENE_SETTINGS})
source_group(Resources/Shader FILES ${SHADER_FILES})

#--- Set include path of external libraries
include_directories(${LIBIGL_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIRS})
include_directories(${JSONCPP_INCLUDE_DIRS})
include_directories(${ASSIMP_INCLUDE_INSTALL_DIR})

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${SCENE_SETTINGS} ${SHADER_FILES} ${UI_FILES} ${QRC_FILE})

#--- Link libraries
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} jsoncpp_static)
target_link_libraries(${PROJECT_NAME} assimp)