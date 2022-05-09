# PhotoPay C SDK

_PhotoPay C SDK_ enables scanning various payment barcodes on payment slips. SDK includes:

- Real-time data extraction
- Offline, on-device processing for maximum security

Integrating PhotoPay C SDK_ into your app requires experience with native development with the C language. If you are looking for native or specific cross-platform SDKs with built-in UX and camera management, please go to the [PhotoPay Github page](https://github.com/PhotoPay).

You should use _PhotoPay C SDK_ if you are developing:

- Windows desktop applications
- C++, C#, Java, Python, Go or Ruby applications (you wrap the C-API in your target language)

# Table of contents

* [System requirements](#system-requirements)
    * [Minimum hardware requirements](#hw-requirements)
    * [Minimum software requirements](#sw-requirements)
* [Integration instructions](#integration)
    * [Package contents](#pkg-contents)
    * [Configuring your build system](#configure-build)
        * [Windows](#configure-windows)
    * [Obtaining a license key](#obtaining-a-license-key)
    * [Performing your first scan](#first-scan)
    * [Thread safety](#thread-safety)
* [The `Recognizer` concept and the `RecognizerRunner`](#available-recognizers)
    * [The `Recognizer` concept](#recognizer-concept)
    * [`RecognizerRunner`](#recognizer-runner)
* [Handling processing events with `RecognitionCallback`](#recognition-callbacks)
* [List of available recognizers](#recognizer-list)
    * [Croatian PDF417 payment barcode recognizer](#croPdf417Recognizer)
    * [Croatian Payment QR code recognizer](#croQrRecognizer)
* [Troubleshooting](#troubleshoot)
    * [Integration problems](#integration-problems)
    * [Licensing problems](#licensing-problems)
        * [Other problems](#other-problems)
* [FAQ and known issues](#faq)
* [Additional info](#info)


# <a name="system-requirements"></a> System requirements

## <a name="hw-requirements"></a> Minimum hardware requirements

### Windows

- _PhotoPay C SDK_ supports any `x86_64` compatible processor
    - x86, ARM, MIPS, SPARC, Power, Itanium and other non-x86_64-compatible CPUs are **not supported**
- 20 MB of available hard disk space
- 128 MB of RAM
    - the software may work even on systems with less RAM, but may fail to perform recognition of very large images

## <a name="sw-requirements"></a> Minimum software requirements

### Windows

- _PhotoPay C SDK_ supports only 64-bit Windows 10
    - 32-bit version of Windows 10, as well as Windows 8.1 and earlier versions are **not supported**
- [Visual C++ 2022 redistributable package](https://aka.ms/vs/17/release/VC_redist.x64.exe) is required for _PhotoPay C SDK_ to work on Windows

# <a name="integration"></a> Integration instructions

## <a name="pkg-contents"></a> Package contents

The _PhotoPay C SDK_ consists of:

- [doxygen documentation](https://photopay.github.io/photopay-c-sdk)
- C headers exposing the public API of the SDK
    - located in [include](include) directory
- dynamic library that contains the implementation of the SDK
    - Windows DLL and import lib is available [here](lib/windows)

## <a name="configure-build"></a> Configuring your build system

In order to be able to use _PhotoPay_ in your application, you first need to configure your build system to be able to find the API headers so that your code will be able to use _PhotoPay's_ API functions. You also need to configure your build system to link with the dynamic library. Since there is no standard build system for C language, we created the most common examples for every OS _PhotoPay C SDK_ supports.

### <a name="configure-windows"></a> Windows

On Windows, _PhotoPay C SDK_ is available as [dynamic library](lib/windows). You need to instruct your compiler to search for headers in [include](include) directory and link with `RecognizerApi.lib`, which is located in [Windows lib folder](lib/windows/x64). When running your app, make sure that `RecognizerApi.dll` is available in the same directory as your application's executable file. In order for _PhotoPay C SDK_ to work, a  [Visual C++ 2022 redistributable package](https://aka.ms/vs/17/release/VC_redist.x64.exe) needs to be installed on the system.

Please check [the Windows sample-app](sample-apps/projects/windows/x64) for an example of integration of _PhotoPay C SDK_ on Windows.

## <a name="obtaining-a-license-key"></a> Obtaining a license key

Using _PhotoPay C SDK_ in your app requires a valid license key.

You can obtain a free trial license key by registering to [Microblink dashboard](https://microblink.com/login). After registering, you will be able to generate a license key for your app.

- On Windows, the license key is bound to the [Windows product ID](https://superuser.com/a/886764)
    - you can obtain the ID by running a [license request tool](lib/windows/LicenseRequestTool.exe). This utility will print the product ID as `Licensee` to the standard output and also into file `MicroblinkLicenseRequest.txt`
    - if you need a Windows license eligible for multiple machines, please [contact us](https://help.microblink.com)

**Keep in mind:** Under our License Management Program a public network access is required.
We’re only asking you to do this so we can validate your trial license key. Scanning or data extraction of data still happens offline, on the device itself. Once the validation is complete, you can continue using the SDK in offline mode (or over a private network) until the next check. If there is a network error during trial license check, functions `recognizerAPIUnlock*` will fail with `MB_RECOGNIZER_ERROR_STATUS_NETWORK_ERROR` error code and functions `recognizerRunnerRecognizeFrom*` will immediately return with `MB_RECOGNIZER_RESULT_STATE_EMPTY`. Note that License Management Program can be disabled for production licenses - in that case no Internet connection will be required for SDK to work.

## <a name="first-scan"></a> Performing your first scan

1. Make sure that you have [set up your build system](#configure-build) and [obtained the license key](#obtaining-a-license-key).

1. Include a main SDK header, which provides all API functions:
    ```c
    #include <RecognizerApi.h>
    ```

1. Define the location of cache folder. This is required to store some cache files when online licenses are used. On mobile platforms those files are stored into app's private folder, but on desktop platforms it is required to define a unique folder in order to avoid clashes with other applications using the same SDK.
    ```c
    recognizerAPISetCacheLocation( "/path/to/cache/folder" );
    ```

1. Insert your license key
    ```c
    MBRecognizerErrorStatus errorStatus = recognizerAPIUnlockWithLicenseKey( "Add license key here" );
    if ( errorStatus != MB_RECOGNIZER_ERROR_STATUS_SUCCESS )
    {
        // handle failure
    }
    ```

1. Create and configure recognizer. For more information about available recognizers, [check here](#available-recognizers)
    ```c
    MBCroatiaPdf417PaymentRecognizer * croatiaPdf417PaymentRecognizer = NULL;

    MBCroatiaPdf417PaymentRecognizerSettings settings;

    // initialize default settings values
    croatiaPdf417PaymentRecognizerSettingsDefaultInit( &settings );

    // optionally tweak settings for your needs

    MBRecognizerErrorStatus errorStatus = = croatiaPdf417PaymentRecognizerCreate( &croatiaPdf417PaymentRecognizer, &settings );
    if ( errorStatus != MB_RECOGNIZER_ERROR_STATUS_SUCCESS )
    {
        // handle failure
    }
    ```

1. Create and configure recognizer runner
    ```c
    MBRecognizerRunnerSettings runnerSett;
    recognizerRunnerSettingsDefaultInit( &runnerSett );

    MBRecognizerPtr recognizers[] = { croatiaPdf417PaymentRecognizer };

    runnerSett.numOfRecognizers = 1;
    runnerSett.recognizers = recognizers;

    MBRecognizerRunner * recognizerRunner = NULL;

    errorStatus = recognizerRunnerCreate( &recognizerRunner, &runnerSett );
    if ( errorStatus != MB_RECOGNIZER_ERROR_STATUS_SUCCESS )
    {
        // handle failure
    }
    ```

1. Prepare the image to be recognized. Image first needs to be created from from memory. To create image from memory buffer use [`recognizerImageCreateFromRawImage`](https://photopay.github.io/photopay-c-sdk/struct_m_b_recognizer_image.html#a15d9ff61beca923011ba0afc0ca9d58f)
    ```c
    int image_width, image_height, image_stride;
    MBByte * image_buffer;

    // populate above variables (i.e. by loading image file or capturing image with camera)

    MBRecognizerImage * img;
    MBRecognizerErrorStatus status = recognizerImageCreateFromRawImage( &img, image_buffer, image_width, image_height, image_stride, MB_RAW_IMAGE_TYPE_BGR );
    if ( status != MB_RECOGNIZER_ERROR_STATUS_SUCCESS ) {
        printf( "Failed to create image. Reason: %s", recognizerErrorToString( status ) );
    }
    ```

    Alternatively, you can load the image from file using the [`recognizerImageLoadFromFile`](https://photopay.github.io/photopay-c-sdk/struct_m_b_recognizer_image.html#ad301891b711bfd06000189b8819021ee)
    ```c
    MBRecognizerImage* img;
    MBRecognizerErrorStatus status = recognizerImageLoadFromFile( &img, "path/to/image.jpg" );
    if ( status != MB_RECOGNIZER_ERROR_STATUS_SUCCESS ) {
        printf( "Failed to load image from file. Reason: %s", recognizerErrorToString( status ) );
    }
    ```

1. Once you have created an image, you can perform recognition using method [`recognizerRunnerRecognizeFromImage`](https://photopay.github.io/photopay-c-sdk/struct_m_b_recognizer_runner.html#a50678935556a9718df4ed4aaf89286c0).
    ```c
    MBRecognizerResultState resultState = recognizerRunnerRecognizeFromImage( recognizerRunner, imageWrapper.recognizerImage, MB_FALSE, NULL );

    if ( resultState != MB_RECOGNIZER_RESULT_STATE_EMPTY )
    {
        // obtain results from recognizers (see Step 4)
    }
    ```

1. Obtain result structure from each of the recognizers. If some recognizer's result's state is [`MB_RECOGNIZER_RESULT_STATE_VALID`](https://photopay.github.io/photopay-c-sdk/_recognizer_8h.html#a5ceb086a453c18e3da30aabbb8406bc0), then it contains recognized data.
    ```c
    MBCroatiaPdf417PaymentRecognizerResult result;

    croatiaPdf417PaymentRecognizerResult( &result, croatiaPdf417PaymentRecognizer );

    if ( result.baseResult.state == MB_RECOGNIZER_RESULT_STATE_VALID )
    {
        // you can use data from the result
    }
    ```

1. Finally, when done, clean the memory. Each structure has method for releasing it.
    ```c
    recognizerImageDelete( &img );
    recognizerRunnerDelete( &recognizerRunner );
    // make sure that recognizers are deleted *AFTER* recognizerRunner
    croatiaPdf417PaymentRecognizerDelete( &croatiaPdf417PaymentRecognizer );
    ```

## <a name="thread-safety"></a> Thread safety

_PhotoPay C SDK_ can be used in a thread-safe manner, however some synchronization may be required on your side. The functions that are used for setting the license key are not thread-safe and should not be used concurrently from different threads. Furthermore, [recognizers and RecognizerRunner](#available-recognizers) should not be shared between different threads without synchronization. Each Recognizer, when associated with `RecognizerRunner` can only be used from the single thread at the time. This means that if you plan to process multiple images in different threads in parallel, you should use specific `RecognizerRunner` and recognizer objects for each of your threads. Failure to do so will result in undefined behaviour. Also, initialization of the `RecognizerRunner` should not be done concurrently with initialization of different `RecognizerRunner` or concurrently with image processing (even with different `RecognizerRunner`). However, once initialized, different instances of `RecognizerRunner` can be safely used concurrently from different threads as long as they are not using the same instances of recognizers.

So, to summarize:

- license key should be set before launching any threads - this is not thread safe
    - this also holds for other global initialization functions, such as [recognizerAPISetCacheLocation](https://photopay.github.io/photopay-c-sdk/_recognizer_api_utils_8h.html#adc944adfb3f0295cd6d905f64292a853)
- each thread needs to have their own set of recognizer objects and `RecognizerRunner`
- [`RecognizerRunner` initialization](https://photopay.github.io/photopay-c-sdk/struct_m_b_recognizer_runner.html#a5c3e9b7dae397fcc7b86853abb3b61f2) must not be performed concurrently neither with initialization of the `RecognizerRunner` on different thread nor with [image processing](https://photopay.github.io/photopay-c-sdk/struct_m_b_recognizer_runner.html#a50678935556a9718df4ed4aaf89286c0)
- [image processing](https://photopay.github.io/photopay-c-sdk/struct_m_b_recognizer_runner.html#a50678935556a9718df4ed4aaf89286c0) can be safely performed concurrently on different threads, provided that:
    - each thread uses its own instance of `RecognizerRunner`
    - recognizer objects are not shared between those instances
- [`RecognizerRunner` cleanup](https://photopay.github.io/photopay-c-sdk/struct_m_b_recognizer_runner.html#a2293aa6a9dafccd7cdc052f7ed47b063) must not be performed concurrently

# <a name="available-recognizers"></a> The `Recognizer` concept and the `RecognizerRunner`

This section will first describe [what is a `Recognizer`](#recognizer-concept) and how it should be used to perform recognition of the still images and video frames. Next, we will describe what is a [`RecognizerRunner`](#recognizer-runner) and how it can be used to tweak the recognition procedure.

## <a name="recognizer-concept"></a> The `Recognizer` concept

The `Recognizer` is the basic unit of processing within the _PhotoPay_ SDK. Its main purpose is to process the image and extract meaningful information from it. As you will see [later](#recognizer-list), the _PhotoPay_ SDK has lots of different `Recognizer` objects that have various purposes.

Each `Recognizer` has a `Result` object, which contains the data that was extracted from the image. The `Result` for each specific `Recognizer` can be obtained by creating a specific `Result` structure (each `Recognizer` has its own unique `Result` structure) and filling its contents by calling the `<recognizerName>RecognizerResult` function (each `Recognizer` has its own unique function for obtaining the results.) The pointers set by the result obtaining function are valid as long as the associated `Recognizer` object is alive, i.e. until `<recognizerName>RecognizerDelete` function is called. Keep that in mind while developing your application.

Every `Recognizer` is a stateful object that can be in two possible states: _idle state_ and _working state_.

While in _idle state_, you are allowed to call `<recognizerName>RecognizerUpdate` function which will update `Recognizer's` properties according to given settings.

After you create a `RecognizerRunner` with array containing your recognizer, the state of the `Recognizer` will change to _working state_, in which `Recognizer` object will be used for processing. While being in _working state_, it is not possible to call function `<recognizerName>RecognizerUpdate` with it as a parameter (calling it will crash your app). If you need to change configuration of your recognizer while its being used, you need to create a new `Recognizer` of the same type with your modified configuration and replace the original `Recognizer` within the `RecognizerRunner` by calling its [`recognizerRunnerUpdateSettings`](https://photopay.github.io/photopay-c-sdk/struct_m_b_recognizer_runner.html#a9477518625bb5348140af7dc9edb85c1) method.

While `Recognizer` object works, it changes its internal state and its result. The `Recognizer` object's `Result` always starts in [`MB_RECOGNIZER_RESULT_STATE_EMPTY`](https://photopay.github.io/photopay-c-sdk/_recognizer_8h.html#a5ceb086a453c18e3da30aabbb8406bc0) state. When corresponding `Recognizer` object performs the recognition of given image, its `Result` can either stay in `MB_RECOGNIZER_RESULT_STATE_EMPTY` state (in case `Recognizer` failed to perform recognition), move to `MB_RECOGNIZER_RESULT_STATE_UNCERTAIN` state (in case `Recognizer` performed the recognition, but not all mandatory information was extracted), move to `MB_RECOGNIZER_RESULT_STATE_STAGE_VALID` (in case multi-stage `Recognizer` performed some stage of the recognition, such as scanning the front side of the document) or move to `MB_RECOGNIZER_RESULT_STATE_VALID` state (in case `Recognizer` performed recognition and all mandatory information was successfully extracted from the image).

## <a name="recognizer-runner"></a> `RecognizerRunner`

The `RecognizerRunner` is the object that manages the chain of individual `Recognizer` objects within the recognition process. It's created by [`recognizerRunnerCreate`](https://photopay.github.io/photopay-c-sdk/struct_m_b_recognizer_runner.html#a5c3e9b7dae397fcc7b86853abb3b61f2) function, which requires a [settings structure](https://photopay.github.io/photopay-c-sdk/struct_m_b_recognizer_runner_settings.html) which contains an array of `Recognizer` objects that will be used for processing and a [`MBBool allowMultipleResults` member](https://photopay.github.io/photopay-c-sdk/struct_m_b_recognizer_runner_settings.html#a09034aebf6ff97ae5f8b66c31f9a6432) indicating whether multiple `Recognizer` objects are allowed to have their `Results` enter the `MB_RECOGNIZER_RESULT_STATE_VALID` state.

To explain further the `allowMultipleResults` parameter, we first need to understand how `RecognizerRunner` performs image processing.

When the [`recognizerRunnerRecognizeFromImage`](https://photopay.github.io/photopay-c-sdk/struct_m_b_recognizer_runner.html#a50678935556a9718df4ed4aaf89286c0) function is called, it processes the image with the first `Recognizer` in chain. If the `Recognizer's` `Result` object changes its state to `MB_RECOGNIZER_RESULT_STATE_VALID`, then if the `allowMultipleResults` parameter is `MB_FALSE`, the recognition chain will be broken and `recognizerRunnerRecognizeFromImage` function will immediately return. If the `allowMultipleResults` parameter is `MB_TRUE`, then the image will also be processed with other `Recognizer` objects in chain, regardless of the state of their `Result` objects. If, after processing the image with the first `Recognizer` in chain, its `Result` object's state is not changed to `Valid`, the `RecognizerRunner` will use the next `Recognizer` object in chain for processing the image and so on - until the end of the chain (if no results become valid or always if `allowMultipleResults` parameter is `MB_TRUE`) or until it finds the `Recognizer` that has successfully processed the image and changed its `Result's` state to `MB_RECOGNIZER_RESULT_STATE_VALID` (if `allowMultipleResults` parameter is `MB_FALSE`).

You cannot change the order of the `Recognizer` objects within the chain - no matter the order in which you give `Recognizer` objects to `RecognizerRunner`, they are internally ordered in a way that provides best possible performance and accuracy.

Also, in order for _PhotoPay_ SDK to be able to order `Recognizer` objects in recognition chain in the best way possible, it is not allowed to have multiple instances of `Recognizer` objects of the same type within the chain. Attempting to do so will crash your application.

# <a name="recognition-callbacks"></a> Handling processing events with `RecognitionCallback`

Processing events, also known as _Recognition callbacks_ are purely intended for giving processing feedback on UI. If you do not plan developing any UI, you will most probably not need to use _Recognition callbacks_.

Callbacks for all possible events are bundled into the [`MBRecognitionCallback`](https://photopay.github.io/photopay-c-sdk/struct_m_b_recognition_callback.html) structure. We suggest checking for more information about available callbacks [in the doxygen documentation](https://photopay.github.io/photopay-c-sdk/struct_m_b_recognition_callback.html).

# <a name="recognizer-list"></a> List of available recognizers

This section will give a list of all `Recognizer` objects that are available within _PhotoPay_ SDK, their purpose and recommendations how they should be used to get best performance and user experience.
## <a name="croPdf417Recognizer"></a> Croatian PDF417 payment barcode recognizer

The [`CroatiaPdf417PaymentRecognizer`](https://photopay.github.io/photopay-c-sdk/struct_m_b_croatia_pdf417_payment_recognizer.html) is recognizer specialized for scanning payment information from 2D PDF417 payment barcodes on croatian payment slips.

In general, you can use the [default settings object](https://photopay.github.io/photopay-c-sdk/struct_m_b_croatia_pdf417_payment_recognizer_settings.html) to configure PDF417 payment barcode recognizer, but in case when you have some special needs, you can change the default settings.

## <a name="croQrRecognizer"></a> Croatian Payment QR code recognizer

The [`CroatiaQrPaymentRecognizer`](https://photopay.github.io/photopay-c-sdk/struct_m_b_croatia_qr_payment_recognizer.html) is recognizer specialized for scanning payment information from payment QR codes found on some croatian payment slips and invoices.

In general, you can use the [default settings object](https://photopay.github.io/photopay-c-sdk/struct_m_b_croatia_qr_payment_recognizer_settings.html) to configure payment QR code recognizer, but in case when you have some special needs, you can change the default settings.
# <a name="troubleshoot"></a> Troubleshooting

## <a name="integration-problems"></a> Integration problems

In case of problems with the integration of the SDK, first make sure that you have tried integrating the SDK exactly as described [in integration instructions](#integration).

If you have followed the instructions to the letter and you still have the problems, please try modifying a sample app so that it will reproduce the issue you are having and then contact us at [help.microblink.com](https://help.microblink.com).

## <a name="licensing-problems"></a> Licensing problems

If you are getting "invalid license key" error or having other license-related problems (e.g. some feature is not enabled that should be), first check the output of your program. All license-related problems are logged to the standard output for each platform (ADB on Android, NSLog on iOS, `stdout` on desktop platforms).

When you have to determine what is the license-related problem or you simply do not understand the log, you should contact us [help.microblink.com](https://help.microblink.com). When contacting us, please make sure you provide following information:

- `licensee` to which your license key is bound
    - for more information about obtaining the `licensee`, check [Obtaining a license key](#obtaining-a-license-key)
- license key that is causing problems
- please stress out that you are reporting problem to the _PhotoPay C SDK_
- please add information about the operating system and its version
- if unsure about the problem, you should also provide excerpt from program output containing the license error

### <a name="other-problems"></a> Other problems

If you are having problems with scanning certain items, undesired behaviour on specific device(s), crashes inside _PhotoPay_ or anything unmentioned, please do as follows:

- Contact us at [help.microblink.com](https://help.microblink.com) describing your problem and provide following information:
    - Log from the program output
    - high resolution scan/photo of the item that you are trying to scan
    - information about the operating system and its version
    - please stress out that you are reporting problem related to the _PhotoPay C SDK_
    - in case of a crash, please provide a crash trace or even a coredump file
# <a name="faq"></a> FAQ and known issues

#### After switching from trial to production license I get error `This entity is not allowed by currently active license!` when I create a specific `Recognizer` object.

Each license key contains information about which features are allowed to use and which are not. This error indicates that your production license does not allow using of specific `Recognizer` object. You should contact [support](https://help.microblink.com) to check if provided license is OK and that it really contains all features that you have purchased.

#### Initialization of the `RecognizerRunner` fails

Please check that you have correctly set the license key and that you have correctly set the path to resources that need to be loaded at runtime. For more information, see [_performing your first scan_ article](#first-scan)

#### Unlocking the SDK fails with `MB_RECOGNIZER_ERROR_STATUS_NETWORK_ERROR`

If you are using trial license key or production license key under License Management Program, it will require Internet connection to periodically validate the license key. Scanning or data extraction of identity documents still happens offline, on the device itself. For more information, check [Obtaining a license key](#obtaining-a-license-key) paragraph.
# <a name="info"></a> Additional info

For any other questions, feel free to contact us at [help.microblink.com](http://help.microblink.com) or open a [new issue on GitHub](https://github.com/BlinkID/blinkid-c-sdk/issues).

## Important links

- [Doxygen documentation](https://photopay.github.io/photopay-c-sdk)
- [Release notes](Release%20notes.md)
- [Demo apps](sample-apps)
- [Microblink help and support](https://help.microblink.com)
    - or open a [new issue on GitHub](https://github.com/PhotoPay/photopay-c-sdk/issues)
