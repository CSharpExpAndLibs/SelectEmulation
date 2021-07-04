using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.IO;
using System.Net.Sockets;
using System.Net;
using System.Runtime.InteropServices;

namespace ConsoleApp1
{

    class Program
    {
        [DllImport("Select", CallingConvention = CallingConvention.StdCall)]
        extern static void InitSelect();
        
        [DllImport("Select", CallingConvention = CallingConvention.StdCall)]
        extern static void SignaleToExit();

        [DllImport("Select", CallingConvention = CallingConvention.StdCall)]
        unsafe extern static char* ReadLine();

        [DllImport("Select", CallingConvention = CallingConvention.StdCall)]
        unsafe extern static void* ReadLinew();

        static AutoResetEvent breakThreadTermEvent = new AutoResetEvent(false);


        static void Main(string[] args)
        {
            Console.WriteLine("Main Starts");
            InitSelect();

            var t = new Thread(BreakThread);
            t.Start();
            Thread.Sleep(500);

            while (true)
            {
                Console.Write("Input> ");
                string line = Read();
                Console.WriteLine($"「{line}」が入力されました");
                if (line.ToLower() == "exit")
                    break;

            }

            // BreakThreadを強制終了
            breakThreadTermEvent.Set();

            Console.WriteLine("何か入力して");
            Console.ReadLine();
        }

        static string Read()
        {
            byte[] buff1;
            unsafe
            {
                uint* buff2 = (uint*)ReadLinew();
                int count = 0;
                for (uint* p = buff2; *p++ != 0; count++) ;
                buff1 = new byte[4 * count];
                for (int i = 0; i < count; i++)
                {
                    for (int j = 0; j < 4; j++)
                        buff1[4 * i + j] = (byte)((buff2[i] >> 8 * j) & 0xff);
                }
            }
            string line = Encoding.Unicode.GetString(buff1);

            return line;
        }

        static void BreakThread()
        {
            Console.WriteLine("--- Braeking Thread Start ---");

            TcpListener server = null;

            try
            {
                // Set the TcpListener on port 13000.
                Int32 port = 13000;
                IPAddress localAddr = IPAddress.Parse("127.0.0.1");

                // TcpListener server = new TcpListener(port);
                server = new TcpListener(localAddr, port);

                // Start listening for client requests.
                server.Start();

                // Buffer for reading data
                Byte[] bytes = new Byte[256];

                // Enter the listening loop.
                while (true)
                {
                    Console.Write("Waiting for a connection... ");

                    // Perform a blocking call to accept requests.
                    // You could also use server.AcceptSocket() here.
                    var acceptTask = server.AcceptTcpClientAsync();
                    var dummy = Task.Run<TcpClient>(() =>
                    {
                        breakThreadTermEvent.WaitOne();
                        // 適当なクライアントを作成する
                        return new TcpClient("", 9999);
                    });
                    TcpClient client =
                        Task.WhenAny<TcpClient>(
                            new Task<TcpClient>[] { acceptTask, dummy }).
                            Result.Result;
                    if (!client.Connected)
                    {
                        Console.WriteLine("BreakThread終了");
                        break;
                    }
                    Console.WriteLine("Connected!");
                    
                    // Get a stream object for reading and writing
                    NetworkStream stream = client.GetStream();

                    string rcvStr = "";
                    int i;

                    // Loop to receive all the data sent by the client.
                    while ((i = stream.Read(bytes, 0, bytes.Length)) != 0)
                    {
                        // Translate data bytes to a ASCII string.
                        rcvStr += System.Text.Encoding.ASCII.GetString(bytes, 0, i);
                    }

                    // Shutdown and end connection
                    client.Close();

                    // "exit"だったらSelectを止める
                    Console.WriteLine($"Message:{rcvStr}");
                    if (rcvStr.ToLower() == "exit")
                    {
                        SignaleToExit();
                        breakThreadTermEvent.Set();
                        break;
                    }
                }
            }
            catch (SocketException e)
            {
                Console.WriteLine("SocketException: {0}", e);
            }
            catch (ThreadAbortException)
            {
                Console.WriteLine("--- BreakerがAbortされた ---");
            }
            finally
            {
                // Stop listening for new clients.
                server.Stop();
            }

            //Thread.Yield();
        }
    }
}
