/*  Code below contains an exact representation of eeprom configuration array
    that is stored inside the flight controller.
    
    Rest of the code extends the DataView low-level interface, that is now included 
    by default inside the javascript engine (chrome v9, firefox v15, IE v10).
    
    You can read more about the DataView interface and its methods
    @ https://developer.mozilla.org/en-US/docs/JavaScript/Typed_arrays/DataView
*/

var eepromConfig = {
    version:              0,
    calibrateESC:         0,
    minimumArmedThrottle: 0,
    
    SENSOR_AXIS_MAP: [0, 0, 0],
    
    ACCEL_BIAS:  [0, 0, 0],
    
    CHANNEL_ASSIGNMENT: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    CHANNEL_FUNCTIONS:  [[0, 0], [0, 0], [0, 0], [0, 0]],
    
    PID_YAW_c:   [0.0, 0.0, 0.0, 0],
    PID_PITCH_c: [0.0, 0.0, 0.0, 0],
    PID_ROLL_c:  [0.0, 0.0, 0.0, 0],
    
    PID_YAW_m:   [0.0, 0.0, 0.0, 0],
    PID_PITCH_m: [0.0, 0.0, 0.0, 0],
    PID_ROLL_m:  [0.0, 0.0, 0.0, 0],
    
    PID_BARO:    [0.0, 0.0, 0.0, 0],
    PID_SONAR:   [0.0, 0.0, 0.0, 0],
    PID_GPS:     [0.0, 0.0, 0.0, 0],
};
var eepromConfigSize;

DataView.prototype.getString = function(str_length, byteOffset) {
    var value = '';
    for (var i = 0; i < str_length; ++i) {
        var char = this.getUint8(byteOffset + i);
        value += String.fromCharCode(char > 127 ? 65533 : char);
    }

    return value;
}

DataView.prototype.getChar = function(byteOffset) {
    return this.getString(1, byteOffset);
}

DataView.prototype.parseUNION = function (structure) {
    var needle = 0;
    
    structure.version = this.getUint8(needle++);
    structure.calibrateESC = this.getUint8(needle++);
    structure.minimumArmedThrottle = this.getUint16(needle, 1);
    needle += 2;
    
    for (var i = 0; i < structure.SENSOR_AXIS_MAP.length; i++) {
        structure.SENSOR_AXIS_MAP[i] = this.getInt16(needle, 1);
        needle += 2;
    }
    
    for (var i = 0; i < structure.ACCEL_BIAS.length; i++) {
        structure.ACCEL_BIAS[i] = this.getInt16(needle, 1);
        needle += 2;
    }
    
    for (var i = 0; i < structure.CHANNEL_ASSIGNMENT.length; i++) {
        structure.CHANNEL_ASSIGNMENT[i] = this.getUint8(needle, 1);
        needle += 1;
    } 

    for (var i = 0; i < structure.CHANNEL_FUNCTIONS.length; i++) {
        structure.CHANNEL_FUNCTIONS[i][0] = this.getUint32(needle, 1);
        structure.CHANNEL_FUNCTIONS[i][1] = this.getUint32(needle + 4, 1);
        needle += 8;
    }      
    
    for (var i = 0; i < 4; i++) {
        structure.PID_YAW_c[i]   = this.getFloat32(needle,       1);
        structure.PID_PITCH_c[i] = this.getFloat32(needle + 16,  1);
        structure.PID_ROLL_c[i]  = this.getFloat32(needle + 32,  1);

        structure.PID_YAW_m[i]   = this.getFloat32(needle + 48,  1);
        structure.PID_PITCH_m[i] = this.getFloat32(needle + 64,  1);
        structure.PID_ROLL_m[i]  = this.getFloat32(needle + 80,  1);

        structure.PID_BARO[i]    = this.getFloat32(needle + 96,  1);
        structure.PID_SONAR[i]   = this.getFloat32(needle + 112, 1);
        structure.PID_GPS[i]     = this.getFloat32(needle + 128, 1);
        
        needle += 4;
    }   
}

DataView.prototype.setUNION = function (structure) {
    var needle = 0;
    
    this.setUint8(needle++, structure.version);
    this.setUint8(needle++, structure.calibrateESC);
    this.setUint16(needle, structure.minimumArmedThrottle, 1);
    needle += 2;

    for (var i = 0; i < structure.SENSOR_AXIS_MAP.length; i++) {
        this.setInt16(needle, structure.SENSOR_AXIS_MAP[i], 1);
        needle += 2;
    }
    
    for (var i = 0; i < structure.ACCEL_BIAS.length; i++) {
        this.setInt16(needle, structure.ACCEL_BIAS[i], 1);
        needle += 2;
    }

    for (var i = 0; i < structure.CHANNEL_ASSIGNMENT.length; i++) {
        this.setUint8(needle, structure.CHANNEL_ASSIGNMENT[i], 1);
        needle += 1;
    }  

    for (var i = 0; i < structure.CHANNEL_FUNCTIONS.length; i++) {
        this.setUint32(needle, structure.CHANNEL_FUNCTIONS[i][0], 1);
        this.setUint32(needle + 4, structure.CHANNEL_FUNCTIONS[i][1], 1);
        needle += 8;
    }
    
    for (var i = 0; i < 4; i++) {
        this.setFloat32(needle, structure.PID_YAW_c[i],        1);
        this.setFloat32(needle + 16, structure.PID_PITCH_c[i], 1);
        this.setFloat32(needle + 32, structure.PID_ROLL_c[i],  1);
        
        this.setFloat32(needle + 48, structure.PID_YAW_m[i],   1);
        this.setFloat32(needle + 64, structure.PID_PITCH_m[i], 1);
        this.setFloat32(needle + 80, structure.PID_ROLL_m[i],  1);
        
        this.setFloat32(needle + 96, structure.PID_BARO[i],    1);
        this.setFloat32(needle + 112, structure.PID_SONAR[i],  1);
        this.setFloat32(needle + 128, structure.PID_GPS[i],    1);
        
        needle += 4;
    }
}

function requestUNION() {
    command_log('Requesting configuration UNION from Flight Controller');
    send_message(PSP.PSP_REQ_CONFIGURATION, 1);
}

function sendUNION() {
    command_log('Sending Configuration UNION to Flight Controller ...');
    
    var eepromConfigBytes = new ArrayBuffer(eepromConfigSize);
    var view = new DataView(eepromConfigBytes, 0);
    view.setUNION(eepromConfig);
    
    var data = new Uint8Array(eepromConfigBytes);
    send_message(PSP.PSP_SET_CONFIGURATION, data);
}