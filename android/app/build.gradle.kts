plugins {
    id("com.android.application")
}

android {
    namespace = "com.julesgrc0.wispy"
    compileSdk = 34
    defaultConfig {
        applicationId = "com.julesgrc0.wispy"
        minSdk = 21
        targetSdk = 34

        versionCode = 5
        versionName = "alpha-0.0.9"
    }

    buildTypes {
        release {
            isJniDebuggable = false
            isMinifyEnabled = true
            isShrinkResources = true

            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
        }
        getByName("debug") {
            isJniDebuggable = true
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
    externalNativeBuild {
        cmake {
            path = file("../../CMakeLists.txt")
        }
    }
    dependenciesInfo {
        includeInApk = false
        includeInBundle = false
    }
    ndkVersion = "26.2.11394342" // "27.0.11718014" because raylib does not support ndk27 (ALooper_pollAll need to be replaced with ALooper_pollOnce)
    buildToolsVersion = "35.0.0 rc4"
}