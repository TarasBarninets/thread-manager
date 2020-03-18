# Thread Manager
Server - generates files by using general threads or threads from the thread pool. Client - receives path to the generated file on its request.
![Thread Manager Client](https://user-images.githubusercontent.com/10049851/76914993-8277f600-68c4-11ea-8a11-9d357cbe8990.PNG)
![Thread Manager Server](https://user-images.githubusercontent.com/10049851/76914974-6d02cc00-68c4-11ea-90b3-fb9142cc5381.PNG)<br/>
The Server gives an ability to generate files in different threads. Generals threads generate files one by one. Requested threads are located in Thread Pool and generate the client requested file with priority.<br/>
The Client gives an ability to receives a path to the requested file. If the requested file not created, it will be created with priority by using thread from thread pool.