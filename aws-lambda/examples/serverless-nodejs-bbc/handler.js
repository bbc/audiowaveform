const { spawnSync } = require("child_process");
const { readFileSync, writeFileSync, unlinkSync } = require("fs");
const AWS = require("aws-sdk");

const s3 = new AWS.S3();

module.exports.bbcAudioWaveForm = async event => {
  if (!event.Records) {
    console.log("not an s3 invocation!");
    return;
  }

  const results = event.Records.map(async record => {
    if (!record.s3) {
      console.log("not an s3 invocation!");
      return Promise.resolve();
    }

    if (!record.s3.object.key.endsWith(".mp3")) {
      console.log("the object isn't a mp3 audio: ", record.s3.object.key);
      return Promise.resolve();
    }

    // get the file
    return s3.getObject({
      Bucket: record.s3.bucket.name,
      Key: record.s3.object.key
    }).promise()
    .then(s3Object => {
      const tmpKey = String(record.s3.object.key).replace(/\//g, '_');
      const newKey = String(record.s3.object.key).replace(process.env.HANDLE_FOLDER, "wave");

      // write file to disk
      writeFileSync(`/tmp/${tmpKey}`, s3Object.Body);

      // generate an audiowaveform!
      spawnSync(
        "/opt/bin/audiowaveform",
        [
          "-i",
          `/tmp/${tmpKey}`,
          "-o",
          `/tmp/${tmpKey}.dat`,
          "-b",
          8
        ],
        { stdio: "inherit" }
      );
      
      // read audiowaveform.dat from disk
      const datFile = readFileSync(`/tmp/${tmpKey}.dat`);

      // delete the temp files
      unlinkSync(`/tmp/${tmpKey}.dat`);
      unlinkSync(`/tmp/${tmpKey}`);

      // upload audiowaveform.dat to s3
      return s3.putObject({
        Bucket: record.s3.bucket.name,
        Key: `${newKey}.dat`,
        Body: datFile
      })
      .promise();
    });
  });
  return Promise.all(results).then(res => {
    console.log('result:', res);
    return res;
  });
};
