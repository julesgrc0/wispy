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


        versionCode = 1
        versionName = "alpha-0.0.5"
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
    ndkVersion = "26.2.11394342"
    buildToolsVersion = "35.0.0 rc1"
}