using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Front_end
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        string pipeName = "ir";
        static int max = 1024;
        byte[] data = new byte[max];
        public MainWindow()
        {
            InitializeComponent();
            //Process.Start("Back-end.exe");
        }

        private void train_Click(object sender, RoutedEventArgs e)
        {
            using (var pipe = new NamedPipeClientStream(pipeName))
            {
                pipe.Connect();
                if (!pipe.IsConnected)
                    throw new Exception("Cannot connect to server");
                string request = "a" + corpus.Text + "\0";
                pipe.Write(Encoding.Unicode.GetBytes(request), 0, 2 * request.Length);
                int read = pipe.Read(data, 0, 1024);
            }
        }

        private void retrieve_Click(object sender, RoutedEventArgs e)
        {
            using (var pipe = new NamedPipeClientStream(pipeName))
            {
                pipe.Connect();
                if (!pipe.IsConnected)
                    throw new Exception("Cannot connect to server");
                string request = "q" + query.Text + "\0";
                pipe.Write(Encoding.Unicode.GetBytes(request), 0, 2 * request.Length);
                int read = pipe.Read(data, 0, 1024);
                byte[] raw_result = new byte[read];
                for (int i = 0; i < read; ++i)
                    raw_result[i] = data[i];
                result.Text = System.Text.Encoding.Unicode.GetString(raw_result);
            }
            Console.WriteLine(result.Text);
        }
    }
}
