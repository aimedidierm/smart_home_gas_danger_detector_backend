<?php

namespace App\Http\Controllers;

use App\Models\Hardware;
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
            return response()->json([
                'message' => 'Status saved in database',
            ], 200);
        }
    }

    /**
     * Display a listing of the resource.
     */
    public function index()
    {
        //
    }

    /**
     * Store a newly created resource in storage.
     */
    public function store(Request $request)
    {
        //
    }

    /**
     * Display the specified resource.
     */
    public function show(Hardware $hardware)
    {
        //
    }

    /**
     * Update the specified resource in storage.
     */
    public function update(Request $request, Hardware $hardware)
    {
        //
    }

    /**
     * Remove the specified resource from storage.
     */
    public function destroy(Hardware $hardware)
    {
        //
    }
}
