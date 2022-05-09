using System;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;

namespace Microblink 
{
    public class Program
    {
        private const string licensee = "test";
        private const string licenseKey = "sRwAAAMEdGVzdFS3yg42TcUzoLpCCqgNcrWogoAYeznNnEQaNwJRo+pZRD57Hff5hqlZsHuJy9xXxtfQgoTiiy7FoMrJVrKKc3fI10xw1umeokz2aos+cWPlR3XjkVqk2Vy4zdoNqLayFjczlW+uOHqbnl26GAP+3KiOQV5kT8LFQeef6rav1Yk/0LrzBTAGVpFott+P0Vnr+Uh9C+k=";

        private const int EXIT_SUCCESS = 0;
        private const int EXIT_FAILURE = 1;

        public static int Main(string[] args)
        {
            /*
             * This is a C# rewrite of the C example found in demo.c source file.
             * The C# wrapper has been generated using SWIG (https://www.swig.org/)
             * Please refer to the C file for documentation and code comments, as
             * it's the only official integration example.
             */

            if ( args.Length < 1 )
            {
                Console.WriteLine("usage: DemoCS.exe <path_to_image>");
                return EXIT_FAILURE;
            }

            RecognizerApiWrapper.recognizerAPISetCacheLocation(".");

            var errorStatus = RecognizerApiWrapper.recognizerAPIUnlockForLicenseeWithLicenseKey(licenseKey, licensee);

            if ( errorStatus != MBRecognizerErrorStatus.MB_RECOGNIZER_ERROR_STATUS_SUCCESS )
            {
                Console.WriteLine("Failed to unlock! Reason: " + RecognizerApiWrapper.recognizerErrorToString(errorStatus));
                return EXIT_FAILURE;
            }

            Bitmap bitmap = new(args[0]);

            // 1. create recognizer image around bitmap
            var bitmapData = bitmap.LockBits(Rectangle.FromLTRB(0, 0, bitmap.Width, bitmap.Height), ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);

            Debug.Assert(bitmapData.PixelFormat == PixelFormat.Format24bppRgb);
            Debug.Assert(bitmapData.Width == bitmap.Width);
            Debug.Assert(bitmapData.Height == bitmap.Height);

            var recognizerImage = RecognizerApiWrapper.new_MBRecognizerImage();
            errorStatus = RecognizerApiWrapper.recognizerImageCreateFromRawImage
            (
                recognizerImage,
                new SWIGTYPE_p_unsigned_char(bitmapData.Scan0, false),
                (ushort)bitmapData.Width,
                (ushort)bitmapData.Height,
                (ushort)bitmapData.Stride,
                MBRawImageType.MB_RAW_IMAGE_TYPE_RGB
            );

            if ( errorStatus != MBRecognizerErrorStatus.MB_RECOGNIZER_ERROR_STATUS_SUCCESS )
            {
                Console.WriteLine( "Failed to wrap C# bitmap. Reason: " + RecognizerApiWrapper.recognizerErrorToString(errorStatus));
                return EXIT_FAILURE;
            }

            // create PDF417 and QR recognizers
            var pdf417Recognizer = RecognizerApiWrapper.new_MBCroatiaPdf417PaymentRecognizer();

            errorStatus = RecognizerApiWrapper.croatiaPdf417PaymentRecognizerCreate(pdf417Recognizer, new MBCroatiaPdf417PaymentRecognizerSettings());
            if ( errorStatus != MBRecognizerErrorStatus.MB_RECOGNIZER_ERROR_STATUS_SUCCESS )
            {
                Console.WriteLine("Failed to create PDF417 recognizer!");
                /* avoid memory leaks - free already allocated things */
                RecognizerApiWrapper.recognizerImageDelete(recognizerImage);
                return EXIT_FAILURE;
            }

            var qrRecognizer = RecognizerApiWrapper.new_MBCroatiaQrPaymentRecognizer();
            errorStatus = RecognizerApiWrapper.croatiaQrPaymentRecognizerCreate(qrRecognizer, new MBCroatiaQrPaymentRecognizerSettings());
            if ( errorStatus != MBRecognizerErrorStatus.MB_RECOGNIZER_ERROR_STATUS_SUCCESS )
            {
                Console.WriteLine("Failed to create QR code recognizer!");
                /* avoid memory leaks - free already allocated things */
                RecognizerApiWrapper.recognizerImageDelete(recognizerImage);
                RecognizerApiWrapper.croatiaPdf417PaymentRecognizerDelete(pdf417Recognizer);
                return EXIT_FAILURE;
            }

            // 2. create and configure recognizer runner with both recognizers
            var recognizerRunner = RecognizerApiWrapper.new_MBRecognizerRunner();
            MBRecognizerRunnerSettings recognizerRunnerSettings = new MBRecognizerRunnerSettings
            {
                allowMultipleResults = (byte)RecognizerApiWrapper.MB_FALSE
            };

            var recognizerArray = RecognizerApiWrapper.new_MBRecognizerArray(2);
            RecognizerApiWrapper.MBRecognizerArray_setitem
            (
                recognizerArray,
                0,
                SWIGTYPE_p_MBCroatiaPdf417PaymentRecognizer.getCPtr(RecognizerApiWrapper.MBCroatiaPdf417PaymentRecognizer_value(pdf417Recognizer)).Handle
            );
            RecognizerApiWrapper.MBRecognizerArray_setitem
            (
                recognizerArray,
                1,
                SWIGTYPE_p_MBCroatiaQrPaymentRecognizer.getCPtr(RecognizerApiWrapper.MBCroatiaQrPaymentRecognizer_value(qrRecognizer)).Handle
            );

            recognizerRunnerSettings.numOfRecognizers = 2;
            recognizerRunnerSettings.recognizers = recognizerArray;

            errorStatus = RecognizerApiWrapper.recognizerRunnerCreate(recognizerRunner, recognizerRunnerSettings);
            if ( errorStatus != MBRecognizerErrorStatus.MB_RECOGNIZER_ERROR_STATUS_SUCCESS )
            {
                Console.WriteLine("Failed to create recognizer runner!");
                /* avoid memory leaks - free already allocated things */
                RecognizerApiWrapper.croatiaPdf417PaymentRecognizerDelete(pdf417Recognizer);
                RecognizerApiWrapper.croatiaQrPaymentRecognizerDelete(qrRecognizer);
                RecognizerApiWrapper.recognizerImageDelete(recognizerImage);
                return EXIT_FAILURE;
            }

            // 3. perform recognition of the image
            var resultState = RecognizerApiWrapper.recognizerRunnerRecognizeFromImage
            (
                RecognizerApiWrapper.MBRecognizerRunner_value(recognizerRunner),
                RecognizerApiWrapper.MBRecognizerImage_value(recognizerImage),
                (byte)RecognizerApiWrapper.MB_FALSE,
                null
            );

            // 4. obtain results from the PDF417 or QR recognizer (whichever has the result)
            bool recognitionSuccessful = false;

            if ( resultState != MBRecognizerResultState.MB_RECOGNIZER_RESULT_STATE_EMPTY )
            {
                MBCroatiaBarcodePaymentRecognizerResult pdf417Result = new();
                MBCroatiaBarcodePaymentRecognizerResult qrResult = new();

                RecognizerApiWrapper.croatiaPdf417PaymentRecognizerResult(pdf417Result, RecognizerApiWrapper.MBCroatiaPdf417PaymentRecognizer_value(pdf417Recognizer));
                RecognizerApiWrapper.croatiaQrPaymentRecognizerResult(qrResult, RecognizerApiWrapper.MBCroatiaQrPaymentRecognizer_value(qrRecognizer));

                if (pdf417Result.baseResult.state == MBRecognizerResultState.MB_RECOGNIZER_RESULT_STATE_VALID )
                {
                    Console.WriteLine("PDF417 result is valid!");
                    Console.WriteLine("IBAN: " + pdf417Result.iban);
                    Console.WriteLine("Amount EUR: " + pdf417Result.amountEur);

                    recognitionSuccessful = true;
                }
                else
                {
                    Console.WriteLine("PDF417 result is invalid!");
                }

                if ( qrResult.baseResult.state == MBRecognizerResultState.MB_RECOGNIZER_RESULT_STATE_VALID )
                {
                    Console.WriteLine("QR result is valid!");
                    Console.WriteLine("IBAN: " + qrResult.iban);
                    Console.WriteLine("Amount EUR: " + qrResult.amountEur);

                    recognitionSuccessful = true;
                }
                else
                {
                    Console.WriteLine("QR result is invalid!");
                }
            }
            else
            {
                Console.WriteLine("Nothing recognizer!");
            }

            // 5. free all allocated resources (C# is just a wrapper around C API and does not perform garbage collection of resources allocated in C)
            RecognizerApiWrapper.recognizerImageDelete(recognizerImage);

            /*
             * NOTE: You must delete Recognizer runner before deleting any of the recognizers it
             *       currently uses. Otherwise a crash will occur.
             */
            RecognizerApiWrapper.recognizerRunnerDelete(recognizerRunner);

            RecognizerApiWrapper.croatiaPdf417PaymentRecognizerDelete(pdf417Recognizer);
            RecognizerApiWrapper.croatiaQrPaymentRecognizerDelete(qrRecognizer);

            return (recognitionSuccessful ? EXIT_SUCCESS : EXIT_FAILURE);
        }
    }
}
