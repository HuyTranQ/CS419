using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Forms;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Client
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            WindowStartupLocation = WindowStartupLocation.CenterScreen;
            InitializeComponent();
            tboxQuery.Focus();
        }

        private String dirPath = "";
        private void btnBrowse_Click(object sender, RoutedEventArgs e)
        {
            /* Select a directory that contains dataset */
            FolderBrowserDialog dialog = new FolderBrowserDialog();
            dialog.ShowNewFolderButton = false;

            DialogResult result = dialog.ShowDialog();

            if (result == System.Windows.Forms.DialogResult.OK)
            {
                dirPath = dialog.SelectedPath;
            }

            tboxDirPath.Text = dirPath;
        }

        private void btnSubmit_Click(object sender, RoutedEventArgs e)
        {
            /* Extract query string */
            String queryString = tboxQuery.Text;
            /* Invalid cases */
            if (String.IsNullOrWhiteSpace(queryString))
            {
                /* Null of empty spaces */
                System.Windows.MessageBox.Show(this, "Invalid query!", "Warning", MessageBoxButton.OK, MessageBoxImage.Warning);
            }
            else if (String.IsNullOrEmpty(dirPath))
            {
                System.Windows.MessageBox.Show(this, "Please select a directory!", "Warning", MessageBoxButton.OK, MessageBoxImage.Warning);
            }
            else
            {
                /* Valid cases */

            }
        }
    }
}
