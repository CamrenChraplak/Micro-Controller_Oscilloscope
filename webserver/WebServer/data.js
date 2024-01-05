const maxReadings = 1000;
let numChannels = 0;
let adcMax = 0;

self.onmessage = function(e) {
    if (Object.hasOwn(e.data, 'numChannels')) {
        numChannels = e.data.numChannels;
        adcMax = e.data.adcMax;
    }
    else if (Object.hasOwn(e.data, 'channels') && numChannels > 0) {
        for (let i = 0; i < numChannels; i++) {
            if (e.data.channels[i].readings.length >= maxReadings) {
                e.data.channels[i].readings.splice(0, 1);
            }
            e.data.channels[i].readings[e.data.channels[i].readings.length] = Math.round(Math.random() * adcMax);
        }
        //self.postMessage(e.data.channels);
        self.postMessage(e.data);
    }
}