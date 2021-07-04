using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.IO;

// Console.ReadLine()で実装を試みたが、ReadLine()でブロックされている
// スレッドを安定してAbortすることが出来ず、断念。Win32PIを用いた低レベル
// IFを使うConsoleApp1にバトンタッチ。
namespace ConsoleApp2
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Main Starts");
            var t = new Thread(HandleConsole);
            AutoResetEvent[] evt = new AutoResetEvent[]
            {
                new AutoResetEvent(false),
                new AutoResetEvent(false),
            };

            t.Start(evt);

            // コンソールで"notify"が入力されるのを待つ
            evt[0].WaitOne();

            // Console.ReadLine()待ちに入るのを待つ
            //Thread.Sleep(500);

            // 標準入力をリダイレクト
            string s = "exit";
            MemoryStream m = new MemoryStream(Encoding.UTF8.GetBytes(s));
            TextReader stdin = Console.In;
            Console.SetIn(new StreamReader(m));

            // コンソールスレッドの待ち解除
            evt[1].Set();

            // コンソールスレッドが終了するのを待つ
            bool b = evt[0].WaitOne(1000);
            if (!b)
            {
                Console.WriteLine("--- NG! ---");
                t.Abort();
                t.Join();
            }

            // 標準入力を元に戻して終了
            Console.SetIn(stdin);

            Console.WriteLine("Input Any Key!");
            Console.ReadLine();

        }

        static void HandleConsole(object evts)
        {
            Console.WriteLine("Thread Start");
            var events = (AutoResetEvent[])evts;
            bool isInNotify = false;

            try
            {
                while (true)
                {

                    Console.Write("Input> ");
                    if (isInNotify)
                    {
                        events[1].WaitOne();
                    }
                    var line = Console.ReadLine();
                    Console.WriteLine($"「{line}」が入力されました");
                    if (line.ToLower() == "exit")
                    {
                        break;
                    }
                    else if (line.ToLower() == "notify")
                    {
                        events[0].Set();
                        isInNotify = true;
                    }
                }
                Console.WriteLine("Exit Loop");
            }
            catch (ThreadAbortException e)
            {
                Console.WriteLine("Thread got Abort!");
            }

            // スレッドが終了したことを親に通知
            events[0].Set();

        }

        static void ExecThread(object evt)
        {
            Console.WriteLine("Thread Start");

            ((AutoResetEvent)evt).WaitOne();

            Console.WriteLine("Thread End");
            
        }
    }
}
