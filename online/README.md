# Online phase: automatic user input inference App demo

## How to use the code

1. Open the code repository with Android studio.
2. Check `local.properties` file and make sure it has correct setup for `sdk.dir`.
3. Adjust `strings.xml` to specify inference upload url. See **RESULT UPLOAD** section below.
4. Examine `cpp/hacklib.cpp` and preload necessary classification model parameters into variables. See **MODEL PRELOADING** section below.


### App UI

The main App interface is empty. Two buttons were at the top to allow
manually controlling start and stop of background GPU data collection
and input inference service.

In this demo, the running service shows a notification notification bar to
show the current inference result.


### MODEL PRELOADING

The output of offline training is a cpp file with name `<devicesetup>.cpp`.
To use the model, follow the instruction in the file comment.

Example:

```
// Usage instruction:
//
// 1. add the following lines into models.h:
//    extern struct perfinfer_model model_<structname>;
// 2. add "&model_<structname>," into model_p list.
```

### RESULT UPLOAD

The App demo may opt to upload the inference result to any website via internet.
Please tweak the `upload_url` string and use an accessible URL. After inference
result is retrieved, the App will try to access `$upload_url/$inferstring` using
HTTPS protocol.

### Note

The external implementation on monitoring the launch of target app is not included.
We reserve related functions `startService()` and `stopService()` to
provide interface to start and stop GPU performance counter collection.
