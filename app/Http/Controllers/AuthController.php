<?php

namespace App\Http\Controllers;

use App\Models\User;
use App\Services\Sms;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Auth;
use Illuminate\Support\Facades\Hash;
use Illuminate\Support\Str;

class AuthController extends Controller
{
    public function mobileLogin(Request $request)
    {
        $request->validate([
            'email' => 'required|email',
            'password' => 'required'
        ]);

        $email = $request->input('email');
        $password = $request->input('password');

        $user = User::where('email', $email)->first();

        if ($user != null) {
            $passwordMatch = Hash::check($password, $user->password);
            if ($passwordMatch) {
                Auth::login($user);
                $token = $user->createToken('token')->accessToken;
                return response()->json([
                    'user' => $user,
                    'token' => $token
                ], 200);
            } else {
                return response()->json(['error' => 'Invalid credentials'], 401);
            }
        } else {
            return response()->json(['error' => 'Invalid email'], 401);
        }
    }

    public function forget(Request $request)
    {
        $request->validate([
            'email' => 'required|string|email'
        ]);

        $user = User::where('email', $request->email)->first();
        if ($user) {
            $newPassword = Str::random(10);
            $message = "Hello, your password had been resseted, you can use " . $newPassword . " to login, Thank you!";
            $sms = new Sms();
            $sms->recipients([$user->phone])
                ->message($message)
                ->sender(env('SMS_SENDERID'))
                ->username(env('SMS_USERNAME'))
                ->password(env('SMS_PASSWORD'))
                ->apiUrl("www.intouchsms.co.rw/api/sendsms/.json")
                ->callBackUrl("");
            $sms->send();
            $user->password = $newPassword;
            $user->update();
            return response()->json(['message' => 'User password reseted'], 200);
        } else {
            return response()->json(['error' => 'Invalid email, User not found'], 401);
        }
    }
}
