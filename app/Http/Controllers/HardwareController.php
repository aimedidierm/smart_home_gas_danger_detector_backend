<?php

namespace App\Http\Controllers;

use App\Models\Hardware;
use App\Models\SmsSend;
use App\Models\User;
use App\Services\Sms;
use Illuminate\Http\Request;

class HardwareController extends Controller
{
    /**
     * Display hardware status from database.
     */
    public function status()
    {
        $data = Hardware::latest()->first();
        return response()->json([
            'valve_open' => $data->valve_open,
            'gas_detected' => $data->gas_detected,
        ], 200);
    }

    /**
     * Update hardware status from database.
     */
    public function updateData(Request $request)
    {
        $request->validate([
            'valve_open' => 'sometimes|boolean',
            'gas_detected' => 'sometimes|boolean'
        ]);

        $currentData = Hardware::latest()->first();
        if ($request->has('valve_open')) {
            $statusUpdate = new Hardware;
            $statusUpdate->gas_detected = $currentData->gas_detected;
            $statusUpdate->valve_open = $request->valve_open;
            $statusUpdate->save();
            return response()->json([
                'message' => 'Status saved in database',
            ], 200);
        }

        if ($request->has('gas_detected')) {
            $statusUpdate = new Hardware;
            $statusUpdate->gas_detected = $request->gas_detected;
            $statusUpdate->valve_open = $currentData->valve_open;
            $statusUpdate->save();
            if ($request->gas_detected == true) {
                $smsStatus = SmsSend::first();
                if ($smsStatus->status == 'not_send') {
                    $smsStatus->status = 'send';
                    $smsStatus->update();
                    $message = "Gas had been detected you can check.";
                    $sms = new Sms();
                    $user = User::first();
                    $sms->recipients([$user->phone])
                        ->message($message)
                        ->sender(env('SMS_SENDERID'))
                        ->username(env('SMS_USERNAME'))
                        ->password(env('SMS_PASSWORD'))
                        ->apiUrl("www.intouchsms.co.rw/api/sendsms/.json")
                        ->callBackUrl("");
                    $sms->send();
                }
            } else {
                $smsStatus = SmsSend::first();
                if ($smsStatus->status == 'send') {
                    $smsStatus->status = 'not_send';
                    $smsStatus->update();
                }
            }
            return response()->json([
                'message' => 'Status saved in database',
            ], 200);
        }
    }
}
