package ai.longev.flutter.flutter_ble;

// Copyright 2017, Paul DeMarco.
// All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.


import android.annotation.TargetApi;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.ScanRecord;
import android.bluetooth.le.ScanResult;
import android.os.Build;
import android.os.ParcelUuid;
import android.util.SparseArray;

import com.google.protobuf.ByteString;


import java.util.List;
import java.util.Map;
import java.util.UUID;

/**
 * Created by paul on 8/31/17.
 */

public class ProtoMaker {

    private static final UUID CCCD_UUID = UUID.fromString("000002902-0000-1000-8000-00805f9b34fb");

    static Protos.ScanResult from(BluetoothDevice device, byte[] advertisementData, int rssi) {
        Protos.ScanResult.Builder p = Protos.ScanResult.newBuilder();
        p.setDevice(from(device));
        if (advertisementData != null && advertisementData.length > 0)
            p.setAdvertisementData(AdvertisementParser.parse(advertisementData));
        p.setRssi(rssi);
        return p.build();
    }

    @TargetApi(21)
    static Protos.ScanResult from(BluetoothDevice device, ScanResult scanResult) {
        Protos.ScanResult.Builder p = Protos.ScanResult.newBuilder();
        p.setDevice(from(device));
        Protos.AdvertisementData.Builder a = Protos.AdvertisementData.newBuilder();
        ScanRecord scanRecord = scanResult.getScanRecord();
        if (Build.VERSION.SDK_INT >= 26) {
            a.setConnectable(scanResult.isConnectable());
        } else {
            if (scanRecord != null) {
                int flags = scanRecord.getAdvertiseFlags();
                a.setConnectable((flags & 0x2) > 0);
            }
        }
        if (scanRecord != null) {
            String deviceName = scanRecord.getDeviceName();
            if (deviceName != null) {
                a.setLocalName(deviceName);
            }
            int txPower = scanRecord.getTxPowerLevel();
            if (txPower != Integer.MIN_VALUE) {
                a.setTxPowerLevel(Protos.Int32Value.newBuilder().setValue(txPower));
            }
            // Manufacturer Specific Data
            SparseArray<byte[]> msd = scanRecord.getManufacturerSpecificData();
            for (int i = 0; i < msd.size(); i++) {
                int key = msd.keyAt(i);
                byte[] value = msd.valueAt(i);
                a.putManufacturerData(key, ByteString.copyFrom(value));
            }
            // Service Data
            Map<ParcelUuid, byte[]> serviceData = scanRecord.getServiceData();
            for (Map.Entry<ParcelUuid, byte[]> entry : serviceData.entrySet()) {
                ParcelUuid key = entry.getKey();
                byte[] value = entry.getValue();
                a.putServiceData(key.getUuid().toString(), ByteString.copyFrom(value));
            }
            // Service UUIDs
            List<ParcelUuid> serviceUuids = scanRecord.getServiceUuids();
            if (serviceUuids != null) {
                for (ParcelUuid s : serviceUuids) {
                    a.addServiceUuids(s.getUuid().toString());
                }
            }
        }
        p.setRssi(scanResult.getRssi());
        p.setAdvertisementData(a.build());
        return p.build();
    }

    static Protos.BluetoothDevice from(BluetoothDevice device) {
        Protos.BluetoothDevice.Builder p = Protos.BluetoothDevice.newBuilder();
        p.setRemoteId(device.getAddress());
        String name = device.getName();
        if (name != null) {
            p.setName(name);
        }
        switch (device.getType()) {
            case BluetoothDevice.DEVICE_TYPE_LE:
                p.setType(Protos.BluetoothDevice.Type.LE);
                break;
            case BluetoothDevice.DEVICE_TYPE_CLASSIC:
                p.setType(Protos.BluetoothDevice.Type.CLASSIC);
                break;
            case BluetoothDevice.DEVICE_TYPE_DUAL:
                p.setType(Protos.BluetoothDevice.Type.DUAL);
                break;
            default:
                p.setType(Protos.BluetoothDevice.Type.UNKNOWN);
                break;
        }
        return p.build();
    }

    static Protos.BluetoothService from(BluetoothDevice device, BluetoothGattService service, BluetoothGatt gatt) {
        Protos.BluetoothService.Builder p = Protos.BluetoothService.newBuilder();
        p.setRemoteId(device.getAddress());
        p.setUuid(service.getUuid().toString());
        p.setIsPrimary(service.getType() == BluetoothGattService.SERVICE_TYPE_PRIMARY);
        for (BluetoothGattCharacteristic c : service.getCharacteristics()) {
            p.addCharacteristics(from(c, gatt));
        }
        for (BluetoothGattService s : service.getIncludedServices()) {
            p.addIncludedServices(from(device, s, gatt));
        }
        return p.build();
    }

    static Protos.BluetoothCharacteristic from(BluetoothGattCharacteristic characteristic, BluetoothGatt gatt) {
        Protos.BluetoothCharacteristic.Builder p = Protos.BluetoothCharacteristic.newBuilder();
        p.setUuid(characteristic.getUuid().toString());
        p.setProperties(from(characteristic.getProperties()));
        if (characteristic.getValue() != null)
            p.setValue(ByteString.copyFrom(characteristic.getValue()));
        for (BluetoothGattDescriptor d : characteristic.getDescriptors()) {
            p.addDescriptors(from(d));
        }
        if (characteristic.getService().getType() == BluetoothGattService.SERVICE_TYPE_PRIMARY) {
            p.setServiceUuid(characteristic.getService().getUuid().toString());
        } else {
            // Reverse search to find service
            for (BluetoothGattService s : gatt.getServices()) {
                for (BluetoothGattService ss : s.getIncludedServices()) {
                    if (ss.getUuid().equals(characteristic.getService().getUuid())) {
                        p.setServiceUuid(s.getUuid().toString());
                        p.setSecondaryServiceUuid(ss.getUuid().toString());
                        break;
                    }
                }
            }
        }
        return p.build();
    }

    static Protos.BluetoothDescriptor from(BluetoothGattDescriptor descriptor) {
        Protos.BluetoothDescriptor.Builder p = Protos.BluetoothDescriptor.newBuilder();
        p.setUuid(descriptor.getUuid().toString());
        p.setCharacteristicUuid(descriptor.getCharacteristic().getUuid().toString());
        p.setServiceUuid(descriptor.getCharacteristic().getService().getUuid().toString());
        if (descriptor.getValue() != null)
            p.setValue(ByteString.copyFrom(descriptor.getValue()));
        return p.build();
    }

    static Protos.CharacteristicProperties from(int properties) {
        return Protos.CharacteristicProperties.newBuilder()
                .setBroadcast((properties & 1) != 0)
                .setRead((properties & 2) != 0)
                .setWriteWithoutResponse((properties & 4) != 0)
                .setWrite((properties & 8) != 0)
                .setNotify((properties & 16) != 0)
                .setIndicate((properties & 32) != 0)
                .setAuthenticatedSignedWrites((properties & 64) != 0)
                .setExtendedProperties((properties & 128) != 0)
                .setNotifyEncryptionRequired((properties & 256) != 0)
                .setIndicateEncryptionRequired((properties & 512) != 0)
                .build();
    }

    static Protos.DeviceStateResponse from(BluetoothDevice device, int state) {
        Protos.DeviceStateResponse.Builder p = Protos.DeviceStateResponse.newBuilder();
        switch (state) {
            case BluetoothProfile.STATE_DISCONNECTING:
                p.setState(Protos.DeviceStateResponse.BluetoothDeviceState.DISCONNECTING);
                break;
            case BluetoothProfile.STATE_CONNECTED:
                p.setState(Protos.DeviceStateResponse.BluetoothDeviceState.CONNECTED);
                break;
            case BluetoothProfile.STATE_CONNECTING:
                p.setState(Protos.DeviceStateResponse.BluetoothDeviceState.CONNECTING);
                break;
            case BluetoothProfile.STATE_DISCONNECTED:
                p.setState(Protos.DeviceStateResponse.BluetoothDeviceState.DISCONNECTED);
                break;
            default:
                break;
        }
        p.setRemoteId(device.getAddress());
        return p.build();
    }
}
