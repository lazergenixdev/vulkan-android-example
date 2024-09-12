require 'vendor/android_studio'

if _ACTION == 'clean' then
    os.rmdir 'build'
end

workspace 'Template1'

configurations { 'Debug', 'Release', 'Dist' }

language 'C++'

location 'build'

if _ACTION == 'android-studio' then
	gradleversion "com.android.tools.build:gradle:8.6.0"
	gradlewrapper {
		"distributionUrl=https://services.gradle.org/distributions/gradle-8.10.1-bin.zip"
	}
	assetpacks { ["pack"] = "install-time", }
	androidnamespace 'com.company.app'
	androidminsdkversion '28'
	androidsdkversion '29'
end

include 'Engine'

include 'App'

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
