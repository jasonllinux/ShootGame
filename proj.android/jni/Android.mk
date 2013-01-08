LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := game_shared

LOCAL_MODULE_FILENAME := libgame

LOCAL_SRC_FILES := hellocpp/main.cpp \
../../Classes/AppDelegate.cpp \
../../Classes/Test//HelloWorldScene.cpp \
../../Classes/Scene//MenuScene.cpp \
../../Classes/Scene//AboutScene.cpp \
../../Classes/Scene//GameScene.cpp \
../../Classes/Scene//SceneManager.cpp \
../../Classes/Utilty//UILayer.cpp \
../../Classes/Entity//GameSprite.cpp \
../../Classes/Entity//AdvanceSprite.cpp \
                   
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes                   

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static cocosdenshion_static cocos_extension_static box2d_static
            
include $(BUILD_SHARED_LIBRARY)

$(call import-module,CocosDenshion/android) \
$(call import-module,cocos2dx) \
$(call import-module,extensions) $(call import-module,external/Box2D)