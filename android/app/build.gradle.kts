plugins {
    id("com.android.application")
}

android {
    signingConfigs {
        create("release") {
        }
    }
    namespace = "com.julesgrc0.wispy"
    compileSdk = 34

    defaultConfig {
        applicationId = "com.julesgrc0.wispy"
        minSdk = 34
        versionCode = 1
        versionName = "alpha-0.0.5"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        release {
            isJniDebuggable = false
            isMinifyEnabled = true
            //isShrinkResources = true
            //multiDexEnabled = true
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
    buildFeatures {
        viewBinding = true
    }
    ndkVersion = "26.2.11394342"
    dependenciesInfo {
        includeInApk = false
        includeInBundle = false
    }
    buildToolsVersion = "35.0.0 rc1"
}

dependencies {

    implementation("androidx.appcompat:appcompat:1.6.1")
    implementation("com.google.android.material:material:1.11.0")
    implementation("androidx.constraintlayout:constraintlayout:2.1.4")
    testImplementation("junit:junit:4.13.2")
    androidTestImplementation("androidx.test.ext:junit:1.1.5")
    androidTestImplementation("androidx.test.espresso:espresso-core:3.5.1")
}