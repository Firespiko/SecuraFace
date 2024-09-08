document.addEventListener('DOMContentLoaded', async () => {
    const modelUrl = '/model/model.onnx.enc'; // URL to fetch the model from


    const openDatabase = () => {
        return new Promise((resolve, reject) => {
            const request = indexedDB.open('ModelDB', 1);

            request.onupgradeneeded = (event) => {
                const db = event.target.result;
                db.createObjectStore('models', { keyPath: 'name' });
            };

            request.onsuccess = (event) => {
                resolve(event.target.result);
            };

            request.onerror = (event) => {
                reject('Database error: ' + event.target.errorCode);
            };
        });
    };


    const storeModel = async (db, modelName, modelData) => {
        return new Promise((resolve, reject) => {
            const transaction = db.transaction(['models'], 'readwrite');
            const store = transaction.objectStore('models');
            const request = store.put({ name: modelName, data: modelData });

            request.onsuccess = () => {
                resolve();
            };

            request.onerror = (event) => {
                reject('Store error: ' + event.target.errorCode);
            };
        });
    };


    const fetchModel = async (url) => {
        const response = await fetch(url);
        if (!response.ok) {
            throw new Error(`Failed to fetch model: ${response.statusText}`);
        }
        return await response.arrayBuffer(); 
    };

    try {
        const db = await openDatabase();
        const modelData = await fetchModel(modelUrl);
        await storeModel(db, 'model.onnx.enc', modelData);
        console.log('Model saved successfully in IndexedDB');
    } catch (error) {
        console.error('Error:', error);
    }
});
