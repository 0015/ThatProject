// worker.js
self.addEventListener('message', function (e) {
   var mean = 0;
   var samples_read = e.data.byteLength / 8;
    if (samples_read > 0) {

        var byteArray = new Int16Array(e.data);

        for (var i = 0; i < samples_read; ++i) {
            mean += (byteArray[i]);
        }

        mean /= samples_read;
        self.postMessage(mean);
    }
}); 