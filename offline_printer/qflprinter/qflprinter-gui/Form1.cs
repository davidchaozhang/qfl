using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace qflprinter_gui
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void PrintDocument(object sender, WebBrowserDocumentCompletedEventArgs ev)
        {
            ((WebBrowser)sender).ShowPrintPreviewDialog();
        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            PrintForm pform = new PrintForm();
            pform.Show();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            ReceiptForm rform = new ReceiptForm();
            rform.Show();
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            WebBrowser wBForPrinting = new WebBrowser();
            wBForPrinting.DocumentCompleted += new WebBrowserDocumentCompletedEventHandler(PrintDocument);
            wBForPrinting.Url = new Uri(Environment.CurrentDirectory + @"\qrres\printtag.html");
        }
    }
}
