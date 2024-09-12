project 'Engine'

kind 'StaticLib'

targetdir '%{wks.location}/bin/%{cfg.system}-%{cfg.buildcfg}'

files { 'src/**' }

includedirs { 'include' }

links { 'vulkan' }

if _ACTION == 'android-studio' then
    files { 'android/*.*' }
    links { 'log', 'android' }

    -- this bad :(
    local cmd = os.translateCommands ( '{COPYDIR} android/jniLibs/ ../build/App/src/main/jniLibs' )
    term.pushColor (term.green)
    print 'Copying Vulkan Validation Layers...'
    term.popColor ()
    os.execute (cmd)

    --androidcmake {
    --    'set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")'
    --}
    --libdirs { "jniLibs/${ANDROID_ABI}/" }
    --links { 'VkLayer_khronos_validation.so' }
end
