if _ACTION == 'android-studio' then
	gradleversion 'com.android.tools.build:gradle:8.6.0'
	gradlewrapper {
		'distributionUrl=https://services.gradle.org/distributions/gradle-8.10.1-bin.zip'
	}
    gradleproperties {
        'android.useAndroidX=true',
    }
	assetpacks { ['pack'] = 'install-time', }
	androidnamespace (settings.namespace)
	androidminsdkversion '34'
	androidsdkversion '34'
    -- fuck you android!
    androidbuildsettings {
        [[configurations.all {
            resolutionStrategy {
                force 'org.jetbrains.kotlin:kotlin-stdlib:1.8.22'
                force 'org.jetbrains.kotlin:kotlin-stdlib-jdk8:1.8.22'
                force 'org.jetbrains.kotlin:kotlin-stdlib-jdk7:1.8.22'
            }
        }]],
    }
    androidabis { 'armeabi-v7a', 'arm64-v8a', 'x86', 'x86_64' }
end