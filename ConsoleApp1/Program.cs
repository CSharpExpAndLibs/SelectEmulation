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
        extern static void Initialize();
        
        [DllImport("Select", CallingConvention = CallingConvention.StdCall)]
        extern static void SignaleToExit();

        [DllImport("Select", CallingConvention = CallingConvention.StdCall)]
        unsafe extern static char* ReadLine();


        static AutoResetEvent breakEvent = new AutoResetEvent(false);
        static string consoleBuffer = null;
        static Thread consoleThread = null;

        static void Main(string[] args)
        {
            Console.WriteLine("Main Starts");
            Initialize();

            var t = new Thread(BreakThread);
            t.Start();
            Thread.Sleep(500);

            while (true)
            {
                string line = Read();
                Console.WriteLine($"「{line}」が入力されました");
                if (line.ToLower() == "exit")
                    break;

            }


            if (t.ThreadState == ThreadState.WaitSleepJoin)
            {
                t.Abort();
                t.Join();
            }

            Console.WriteLine(t.ThreadState.ToString());
            Console.WriteLine("何か入力して");
            Console.ReadLine();
        }

        static AutoResetEvent inputEvent = null;
        static AutoResetEvent readyEvent = null;

        static string Read()
        {
            byte[] buff1;
            unsafe
            {
                char* buff2 = ReadLine();
                int count = 0;
                for (; *buff2++ != '\0'; count++) ;
                buff1 = new byte[count];
                for (int i = 0; i < count; count++)
                    buff1[i] = (byte)buff2[i];
            }
            string line = Encoding.UTF8.GetString(buff1);

            return line;
        }


        static void HandleConsole(object evts)
        {
            Console.WriteLine("Console Thread Start");
            var events = (AutoResetEvent[])evts;

            try
            {
                while (true)
                {
                    Console.Write("Input> ");
                    var line = Console.ReadLine();
                    consoleBuffer = line;
                    events[0].Set();
                    events[1].WaitOne();
                }
            }
            catch (ThreadAbortException e)
            {
                Console.WriteLine("--- Console Thread got Abort! ---");
            }

            Thread.Yield();
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
                    TcpClient client = server.AcceptTcpClientAsync().Result;
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
