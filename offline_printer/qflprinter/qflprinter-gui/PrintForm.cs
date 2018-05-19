using System;
using System.IO;
using System.Text;
using System.Runtime.InteropServices;
using System.Drawing.Printing;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace qflprinter_gui
{
    public partial class PrintForm : Form
    {
        [DllImport("qflprinter-dll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void OfflinePrint();

        [DllImport("qflprinter-dll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PrintTag(StringBuilder cpyStr, int ID);

        [DllImport("qflprinter-dll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PrintReceipt(StringBuilder cpyStr, int ID);
        public PrintForm()
        {
            InitializeComponent();
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label3_Click(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }

        //Print Button
        private void button1_Click(object sender, EventArgs e)
        {
           /* PrintDocument pd = new PrintDocument();
            pd.DefaultPageSettings.PaperSize = new PaperSize("Shipping", 398, 213);
            pd.PrintPage += new PrintPageEventHandler(this.pd_PrintPage);
            pd.Print();*/

            WebBrowser wBForPrinting = new WebBrowser();
            wBForPrinting.DocumentCompleted += new WebBrowserDocumentCompletedEventHandler(PrintDocument);
            wBForPrinting.Url = new Uri(Environment.CurrentDirectory + @"\qrres\printtag.html");
        }
        private void PrintDocument(object sender, WebBrowserDocumentCompletedEventArgs ev)
        {
            //((WebBrowser)sender).ShowPrintPreviewDialog();
            ((WebBrowser)sender).Print();
            ((WebBrowser)sender).Dispose();
        }

        private void button2_Click(object sender, EventArgs e)
        {

        }

        private void label2_Click_1(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged_1(object sender, EventArgs e)
        {

        }

        private void label3_Click_1(object sender, EventArgs e)
        {

        }

        private void load_Click(object sender, EventArgs e)
        {
            if (loadid.Text != "")
            {
                if(loadid.Text.IndexOf(",") != -1)
                {
                    loadid.Text = loadid.Text.Substring(0, loadid.Text.IndexOf(","));
                }
                StringBuilder res = new StringBuilder(256);
                PrintTag(res, Convert.ToInt32(loadid.Text));
                string str = res.ToString();
                string[] fillString = str.Split(',');
                id_field.Text = fillString[0]; //Personal ID
                pid_field.Text = fillString[1];
                church_field.Text = fillString[2];
                fname.Text = fillString[5];
                lname.Text = fillString[6];
                cname.Text = fillString[7];
                group_field.Text = fillString[9];
                needRoom.Text = fillString[10];
                age.Text = fillString[11];
                gender.Text = fillString[12];
                grade.Text = fillString[13];
                //registrationFee.Text = "$0"; //not on the excel right now
                //keyFee.Text = "$" + fillString[15];
            }
        }

        private void FirstN_Click(object sender, EventArgs e)
        {

        }

        private void fname_TextChanged(object sender, EventArgs e)
        {

        }

        private void label4_Click(object sender, EventArgs e)
        {

        }

        private void label5_Click(object sender, EventArgs e)
        {

        }

        private void button_checkin_Click(object sender, EventArgs e)
        {

        }

        private void loadid_TextChanged(object sender, EventArgs e)
        {

        }

        private void registrationFee_TextChanged(object sender, EventArgs e)
        {

        }

        private void label6_Click(object sender, EventArgs e)
        {

        }

        private void PrintForm_Load(object sender, EventArgs e)
        {

        }

        private void needRoom_TextChanged(object sender, EventArgs e)
        {

        }

        private void keyFee_TextChanged(object sender, EventArgs e)
        {

        }

        private void label6_Click_1(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged_2(object sender, EventArgs e)
        {

        }
    }
}
