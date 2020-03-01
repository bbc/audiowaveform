'use strict';
const { spawnSync } = require("child_process");
const { readFileSync, writeFileSync, unlinkSync } = require("fs");
const AWS = require("aws-sdk");

const s3 = new AWS.S3();

module.exports.bbcAudioWaveForm = async event => {
  if (!event.Records) {
    console.log("not an s3 invocation!");
    return;
  }

  const results = event.Records.map(record => {
    if (!record.s3) {
      console.log("not an s3 invocation!");
      continue;
    }

    if (!record.s3.object.key.endsWith(".mp3")) {
      console.log("the object isn't a mp3 audio: ", record.s3.object.key);
      continue;
    }

    // get the file
    return s3.getObject({
      Bucket: record.s3.bucket.name,
      Key: record.s3.object.key
    }).promise()
    .then(s3Object => {
      // write file to disk
      writeFileSync(`/tmp/${record.s3.object.key}`, s3Object.Body);

      // generate an audiowaveform!
      spawnSync(
        "/opt/bin/audiowaveform",
        [
          "-i",
          `/tmp/${record.s3.object.key}`,
          "-o",
          `/tmp/${record.s3.object.key}.dat`,
          "-b",
          8
        ],
        { stdio: "inherit" }
      );
      
      // read audiowaveform.dat from disk
      const datFile = readFileSync(`/tmp/${record.s3.object.key}.dat`);

      // delete the temp files
      unlinkSync(`/tmp/${record.s3.object.key}.dat`);
      unlinkSync(`/tmp/${record.s3.object.key}`);

      const newKey = new String(record.s3.object.key).replace(process.env.HANDLE_FOLDER, "wave");

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
