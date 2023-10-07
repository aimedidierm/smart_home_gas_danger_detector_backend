<?php

namespace App\Services;

use Aimedidierm\IntouchSms\SmsAbstract;

class Sms extends SmsAbstract
{
    public function __construct()
    {
        parent::__construct();

        //
    }

    public function configSender(): string
    {
        return "intouchSenderId";
    }

    public function configUsername(): string
    {
        return "intouchUsername";
    }

    public function configPassword(): string
    {
        return "intouchPassword";
    }

    public function configApiUrl(): string
    {
        return "www.intouchsms.co.rw/api/sendsms/.json";
    }

    public function configCallBackUrl(): string
    {
        return "";
    }


    public static function QuickSend($recipients, String $message, String $senderId = null)
    {
        $sms = new Sms();
        $sms->requiredData($recipients, $message, $senderId);
        return $sms->send();
    }
}
