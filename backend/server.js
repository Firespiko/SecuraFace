const express = require('express');
const path = require('path');
const app = express();
const port = 3000;


app.use(express.static(path.join(__dirname, '../public')));


app.get('/model/:modelName', (req, res) => {
    const modelPath = path.join(__dirname, 'model', req.params.modelName);
    console.log('Model Path:', modelPath); 
    res.sendFile(modelPath);
});


app.listen(port, () => {
    console.log(`ONNX model server running at http://localhost:${port}`);
});
