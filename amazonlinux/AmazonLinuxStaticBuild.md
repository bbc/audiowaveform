# Amazon Linux
## working 15 Oct 2024 - built 1.10.1
For running as a lambda function, you ideally want a samll deployment.
The Dockerfile here will create a statically linked binary.

Move the Dockerfile to your dirctory, install docker and then:
```
docker build -t static-aws-audiowaveform .
docker create --name temp-container static-aws-audiowaveform
docker cp temp-container:/audiowaveform.zip .
docker rm temp-container
```
On my Intel Atom potato it takes about 30mins to complete the build