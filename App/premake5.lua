
project 'App'

kind 'WindowedApp'

targetdir '%{wks.location}/bin/%{cfg.system}-%{cfg.buildcfg}'

files { 'src/**' }

includedirs { '../Engine/include' }

links { 'Engine' }

if _ACTION == 'android-studio' then
    files { 'android/**' }
    buildoptions { '-std=c++17' }
    androidabis { 'armeabi-v7a', 'arm64-v8a', 'x86', 'x86_64' }
    assetpackdependencies { 'pack' }
	androiddependencies {
	--	'com.android.support:support-v4:27.1.0',
    --  'androidx.core:core:1.5.0',
	--	'androidx.games:games-activity:3.0.5',
	}
    --androidcmake {
    --    'target_include_directories(App PRIVATE ${ANDROID_NDK}/sources/android/native_app_glue)',
    --    'set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")',
    --    'add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)',
    --    'target_link_libraries(App android native_app_glue EGL GLESv1_CM log)',
    --}
end
