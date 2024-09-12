include 'scripts/premake-android.lua'

defines { ('__TITLE__=\"%s\"'):format(settings.title) }

project 'Engine'

kind 'StaticLib'

targetdir '%{wks.location}/bin/%{cfg.system}-%{cfg.buildcfg}'

files { 'src/**' }

includedirs { 'include' }

if _ACTION == 'android-studio' then
    files { 'android/Activity.cpp', }
    links { 'log', 'android', 'vulkan' }

    defines { ('__ANDROID_NAMESPACE__=%s'):format(settings.namespace:gsub('%.', '_')) }

    -- this bad :(    hard-coded project name
    local cmd = os.translateCommands ( '{COPYDIR} android/jniLibs/ ../' .. settings.build_location .. '/App/src/main/jniLibs' )
    term.pushColor (term.blue)
    print 'Copying Vulkan Validation Layers...'
    os.execute (cmd)
    term.popColor ()
else -- Windows
    VULKAN_SDK = os.getenv("VULKAN_SDK")
    if VULKAN_SDK == nil then
        error("Must have Vulkan SDK installed")
    end

    includedirs { 'include', VULKAN_SDK .. '/Include' }
    files { 'windows/**' }
end

engineinclude = function ()
    includedirs { '../Engine/include' }
    if VULKAN_SDK then
        includedirs { VULKAN_SDK .. '/Include' }
        links { 'vulkan-1' }
        libdirs { '%{VULKAN_SDK}/Lib' }
    end
end

android = function (info)
    if _ACTION ~= 'android-studio' then return end;

    term.pushColor (term.blue)
    print 'Generating Activity.java...'
    os.execute (
        string.format (
            "python ../Engine/scripts/template.py ../Engine/android/MainActivity.template.java \"{'name': '%s', 'namespace': '%s'}\" -O \"%s\"",
            info.name,
            settings.namespace,
            '../' .. settings.build_location .. '/' .. info.name .. '/src/java/'
        )
    )
    term.popColor ()
    
    term.pushColor (term.blue)
        print 'Generating strings.xml...'
        os.mkdir ('../' .. settings.build_location .. '/' .. info.name .. '/res/values/')
        io.writefile (
            '../' .. settings.build_location .. '/' .. info.name .. '/res/values/strings.xml',
            '<resources><string name="title">' .. settings.title .. '</string></resources>'
        )
        files ('../' .. settings.build_location .. '/' .. info.name .. '/res/values/*')
    term.popColor ()
    
    files { '../' .. settings.build_location .. '/' .. info.name .. '/src/java/*' }
    files { '../Engine/android/AndroidManifest.xml' }
    files { '../Engine/android/res/**' }

    buildoptions { '-std=c++17' }
    androiddependencies {
        'androidx.appcompat:appcompat:1.7.0'
    }
    assetpackdependencies { 'pack' }
end
