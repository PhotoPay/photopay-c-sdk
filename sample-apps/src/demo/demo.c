/**
* Copyright (c) Microblink Ltd. All rights reserved.
*
* ANY UNAUTHORIZED USE OR SALE, DUPLICATION, OR DISTRIBUTION
* OF THIS PROGRAM OR ANY OF ITS PARTS, IN SOURCE OR BINARY FORMS,
* WITH OR WITHOUT MODIFICATION, WITH THE PURPOSE OF ACQUIRING
* UNLAWFUL MATERIAL OR ANY OTHER BENEFIT IS PROHIBITED!
* THIS PROGRAM IS PROTECTED BY COPYRIGHT LAWS AND YOU MAY NOT
* REVERSE ENGINEER, DECOMPILE, OR DISASSEMBLE IT.
*/

#include "Recognizer/RecognizerImage.h"
#include <LicenseKey.h>

#include <RecognizerApi.h>

#include <stdio.h>
#include <stdlib.h>

int main( int argc, char * argv[] )
{
    /* Recognizer that can scan PDF417 payment barcodes */
    MBCroatiaPdf417PaymentRecognizer * pdf417Recognizer = NULL;
    /* Recognizer that can scan payment QR codes */
    MBCroatiaQrPaymentRecognizer * qrRecognizer = NULL;
    /* Recognizer runner is the main object that orchestrates recognition using one or more recognizers */
    MBRecognizerRunner * recognizerRunner = NULL;
    /* This is the image that will be processed */
    MBRecognizerImage * image = NULL;
    /* Most of the functions from the Recognizer API return error status to indicate whether operation succeeded or not */
    MBRecognizerErrorStatus errorStatus;

    MBBool recognitionSuccessfull = MB_FALSE;

    if (argc < 2)
    {
        printf("usage %s <path_to_image> \n", argv[0]);
        return EXIT_FAILURE;
    }

    recognizerAPISetCacheLocation( "." );

    /* Unlock the Recognizer API with your license key */

#if defined LICENSE_KEY && defined LICENSEE
    errorStatus = recognizerAPIUnlockForLicenseeWithLicenseKey( LICENSE_KEY, LICENSEE );
#elif defined LICENSE_KEY
    errorStatus = recognizerAPIUnlockWithLicenseKey( LICENSE_KEY );
#else
    /* insert license key and licensee */
    errorStatus = recognizerAPIUnlockForLicenseeWithLicenseKey( "Add license key here", "Add licensee here" );
    /* OR insert license key for licensee obtained with LicenseRequestTool */
    errorStatus = recognizerAPIUnlockWithLicenseKey( "Add license key here" );
#endif

    if ( errorStatus != MB_RECOGNIZER_ERROR_STATUS_SUCCESS )
    {
        printf( "Failed to unlock! Reason: %s\n", recognizerErrorToString( errorStatus ) );
        return EXIT_FAILURE;
    }

    /* load the image from given path */
    errorStatus = recognizerImageLoadFromFile( &image, argv[ 1 ] );
    if ( errorStatus != MB_RECOGNIZER_ERROR_STATUS_SUCCESS )
    {
        printf( "Failed to load image '%s'. Reason: %s\n", argv[ 1 ], recognizerErrorToString( errorStatus ) );
        return EXIT_FAILURE;
    }

    /* Step 1: Create and configure PDF417 and Qr recognizers */
    {
        /*
         * PDF417 payment recognizers settings structure contains all configuration options that are available for the PDF417 payment recognizer.
         * Please note that in C++ a default constructor will be invoked when instantiating this structure, but in C code
         * you need to set all values manually, or use croatiaPdf417PaymentRecognizerSettingsInit function, like here.
         */
        MBCroatiaPdf417PaymentRecognizerSettings pdf417Sett;

        croatiaPdf417PaymentRecognizerSettingsInit( &pdf417Sett );

        /* To create PDF417 recognizer using settings structure, you shuold call croatiaPdf417PaymentRecognizerCreate function. */
        errorStatus = croatiaPdf417PaymentRecognizerCreate( &pdf417Recognizer, &pdf417Sett );
        if ( errorStatus != MB_RECOGNIZER_ERROR_STATUS_SUCCESS )
        {
            printf( "Failed to create PDF417 recognizer!\n" );
            /* avoid memory leaks - free already allocated things */
            recognizerImageDelete( &image );
            return EXIT_FAILURE;
        }

        /* In a similar manner, initialize the payment QR code recognizer */
        MBCroatiaQrPaymentRecognizerSettings qrSett;
        croatiaQrPaymentRecognizerSettingsInit( &qrSett );

        errorStatus = croatiaQrPaymentRecognizerCreate( &qrRecognizer, &qrSett );
        if ( errorStatus != MB_RECOGNIZER_ERROR_STATUS_SUCCESS )
        {
            printf( "Failed to create QR code recognizer!\n" );
            /* avoid memory leaks - free already allocated things */
            croatiaPdf417PaymentRecognizerDelete( &pdf417Recognizer );
            recognizerImageDelete( &image );
            return EXIT_FAILURE;
        }
    }

    /* Step 2: Create and configure recognizer runner with both PDF417 and QR code recognizers */
    {
        /*
         * Recognizer runner needs to be configured with MBRecognizerRunnerSettings structure, that will give
         * information about which recognizers should be used and whether or not multiple results per image
         * is allowed.
         */
        MBRecognizerRunnerSettings runnerSett;

        /* Initialize structure to default values */
        recognizerRunnerSettingsDefaultInit( &runnerSett );

        /*
         * This is an array of pointers to recognizers that will be used to pass information about desired
         * recognizers to the recognizer runner.
         */
        MBRecognizerPtr recognizers[ 2 ];

        runnerSett.allowMultipleResults = MB_FALSE;
        recognizers[ 0 ] = pdf417Recognizer;
        recognizers[ 1 ] = qrRecognizer;

        runnerSett.numOfRecognizers = 2;
        runnerSett.recognizers = recognizers;

        errorStatus = recognizerRunnerCreate( &recognizerRunner, &runnerSett );
        if ( errorStatus != MB_RECOGNIZER_ERROR_STATUS_SUCCESS )
        {
            printf( "Failed to create recognizer runner!\n" );
            /* avoid memory leaks - free already allocated things */
            croatiaPdf417PaymentRecognizerDelete( &pdf417Recognizer );
            croatiaQrPaymentRecognizerDelete( &qrRecognizer );
            recognizerImageDelete( &image );
            return EXIT_FAILURE;
        }
    }

    /* Step 3: Perform recognition of the image */
    {
        MBRecognizerResultState resultState;

        resultState = recognizerRunnerRecognizeFromImage( recognizerRunner, image, MB_FALSE, NULL );

        /* if at least one recognizer produced a result */
        if ( resultState != MB_RECOGNIZER_RESULT_STATE_EMPTY )
        {
            /* Step 4: obtain results from the PDF417 or QR recognizer (whichever has the result) */
            MBCroatiaPdf417PaymentRecognizerResult pdf417Result;
            MBCroatiaQrPaymentRecognizerResult     qrResult;

            croatiaPdf417PaymentRecognizerResult( &pdf417Result, pdf417Recognizer );
            croatiaQrPaymentRecognizerResult    ( &qrResult    , qrRecognizer     );

            if ( pdf417Result.baseResult.state == MB_RECOGNIZER_RESULT_STATE_VALID )
            {
                printf( "PDF417 result is valid!\n" );
                printf( "IBAN: %s.\n", pdf417Result.iban );
                printf( "Amount EUR: %d\n"  , pdf417Result.amountEur );

                recognitionSuccessfull = MB_TRUE;
            }
            else
            {
                printf( "PDF417 result is invalid!\n" );
            }

            if ( qrResult.baseResult.state == MB_RECOGNIZER_RESULT_STATE_VALID )
            {
                printf( "QR result is valid!\n" );
                printf( "IBAN: %s.\n", qrResult.iban );
                printf( "Amount EUR: %d\n"  , qrResult.amountEur );

                recognitionSuccessfull = MB_TRUE;
            }
            else
            {
                printf( "QR result is invalid!\n" );
            }
        }
        else
        {
            printf( "Nothing recognized!\n" );
        }
    }

    /* Step 5: free all allocated resources */
    recognizerImageDelete( &image );

    /*
     * NOTE: You must delete Recognizer runner before deleting any of the recognizers it
     *       currently uses. Otherwise a crash will occur.
     */
    recognizerRunnerDelete( &recognizerRunner );

    croatiaPdf417PaymentRecognizerDelete( &pdf417Recognizer );
    croatiaQrPaymentRecognizerDelete    ( &qrRecognizer     );

    return ( recognitionSuccessfull ? EXIT_SUCCESS : EXIT_FAILURE );
}
