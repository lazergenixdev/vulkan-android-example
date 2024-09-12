require 'vendor/android_studio'

defines { 'ENGINE_NAME="Engine"' }

if _ACTION == 'clean' then
    os.rmdir 'build'
end

settings = {
    build_location = './build/' .. _ACTION,
    namespace = 'com.lazergenixdev.Example',
    title = 'Android Vulkan Example',
}

workspace 'Example'

architecture 'x86_64'

configurations { 'Debug', 'Release', 'Dist' }

language 'C++'

cppdialect 'c++20'

location (settings.build_location)

filter 'configurations:Debug'
    defines { 'CONFIG_DEBUG' }
    symbols 'On'
filter 'configurations:Release'
    defines { 'CONFIG_RELEASE' }
    symbols 'On'
    optimize 'On'
filter 'configurations:Dist'
    defines { 'CONFIG_DIST' }
    optimize 'On'
filter {}

include 'Engine'

include 'App'
