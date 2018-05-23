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
    public partial class ReceiptForm : Form
    {
        [DllImport("qflprinter-dll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void OfflinePrint();

        [DllImport("qflprinter-dll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PrintTag(StringBuilder cpyStr, int ID);

        [DllImport("qflprinter-dll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PrintReceipt(StringBuilder cpyStr, int ID);
        private int numPeople;
        private bool[] checkID;

        //constructor [read in data]
        public ReceiptForm()
        {
            this.MinimumSize = new System.Drawing.Size(this.Width, this.Height);
            // no larger than screen size
            this.MaximumSize = new System.Drawing.Size(Screen.PrimaryScreen.Bounds.Width, Screen.PrimaryScreen.Bounds.Height);
            this.AutoSize = true;
            this.AutoSizeMode = AutoSizeMode.GrowAndShrink;

            checkID = new bool[10000]; //arbitrary large number
            string[] lines = System.IO.File.ReadAllLines(Environment.CurrentDirectory + @"\qrres\qfl_checkedin-data.csv");
            if(!(lines.Length < 2))
                for (int i = 1; i < checkID.Length; i++)
                {
                    if (i >= lines.Length)
                        break;
                    string[] data = lines[i].Split(',');
                    checkID[Convert.ToInt32(data[0])] = true;
                }
            
            InitializeComponent();
        }

        //functions for utility (not related to gui)
        private void ClearTextBoxes()
        {
            Action<Control.ControlCollection> func = null;

            func = (controls) =>
            {
                foreach (Control control in controls)
                    if (control is TextBox && control != barcode)
                        (control as TextBox).Clear();
                    else
                        func(control.Controls);
            };

            func(Controls);
        }
        private void adjustDues()
        {
            for (int i = 0; i < numPeople; i++)
            {
                switch (i + 1)
                {
                    case 1:
                        partyFee.Text = adjustedFee1.Text;
                        break;
                    case 2:
                        partyFee.Text = Convert.ToString(Convert.ToDouble(partyFee.Text) + Convert.ToDouble(adjustedFee2.Text));
                        break;
                    case 3:
                        partyFee.Text = Convert.ToString(Convert.ToDouble(partyFee.Text) + Convert.ToDouble(adjustedFee3.Text));
                        break;
                    case 4:
                        partyFee.Text = Convert.ToString(Convert.ToDouble(partyFee.Text) + Convert.ToDouble(adjustedFee4.Text));
                        break;
                    case 5:
                        partyFee.Text = Convert.ToString(Convert.ToDouble(partyFee.Text) + Convert.ToDouble(adjustedFee5.Text));
                        break;
                    case 6:
                        partyFee.Text = Convert.ToString(Convert.ToDouble(partyFee.Text) + Convert.ToDouble(adjustedFee6.Text));
                        break;
                    case 7:
                        partyFee.Text = Convert.ToString(Convert.ToDouble(partyFee.Text) + Convert.ToDouble(adjustedFee7.Text));
                        break;
                }

            }
            if (partyPaid.Text.Length >= 1)
                adjustedDue.Text = Convert.ToString(Convert.ToDouble(partyFee.Text) - Convert.ToDouble(partyPaid.Text));
            else
                adjustedDue.Text = partyFee.Text;
        }
        private void resetPaid()
        {
            for (int i = 0; i < numPeople; i++)
            {
                switch (i + 1)
                {
                    case 1:
                        if (regfee1.Text.Length < 1)
                            regfee1.Text = "0";
                        if (keyFee1.Text.Length < 1)
                            keyFee1.Text = "0";
                        if (adjustedFee1.Text.Length < 1)
                            adjustedFee1.Text = "0";
                        break;
                    case 2:
                        if (regfee2.Text.Length < 1)
                            regfee2.Text = "0";
                        if (keyFee2.Text.Length < 1)
                            keyFee2.Text = "0";
                        if (adjustedFee2.Text.Length < 1)
                            adjustedFee2.Text = "0";
                        break;
                    case 3:
                        if (regfee3.Text.Length < 1)
                            regfee3.Text = "0";
                        if (keyFee3.Text.Length < 1)
                            keyFee3.Text = "0";
                        if (adjustedFee3.Text.Length < 1)
                            adjustedFee3.Text = "0";
                        break;
                    case 4:
                        if (regfee4.Text.Length < 1)
                            regfee4.Text = "0";
                        if (keyFee4.Text.Length < 1)
                            keyFee4.Text = "0";
                        if (adjustedFee4.Text.Length < 1)
                            adjustedFee4.Text = "0"; ;
                        break;
                    case 5:
                        if (regfee5.Text.Length < 1)
                            regfee5.Text = "0";
                        if (keyFee5.Text.Length < 1)
                            keyFee5.Text = "0";
                        if (adjustedFee5.Text.Length < 1)
                            adjustedFee5.Text = "0";
                        break;
                    case 6:
                        if (regfee6.Text.Length < 1)
                            regfee6.Text = "0";
                        if (keyFee6.Text.Length < 1)
                            keyFee6.Text = "0";
                        if (adjustedFee6.Text.Length < 1)
                            adjustedFee6.Text = "0";
                        break;
                    case 7:
                        if (regfee7.Text.Length < 1)
                            regfee7.Text = "0";
                        if (keyFee7.Text.Length < 1)
                            keyFee7.Text = "0";
                        if (adjustedFee7.Text.Length < 1)
                            adjustedFee7.Text = "0";
                        break;
                    default:
                        break;
                }

            }
        }
        private void PrintDocument(object sender, WebBrowserDocumentCompletedEventArgs ev)
        {
            //((WebBrowser)sender).ShowPrintPreviewDialog();
            ((WebBrowser)sender).Print();
            ((WebBrowser)sender).Dispose();
        }
        public string ReplaceFirst(string text, string search, string replace)
        {
            int pos = text.IndexOf(search);
            if (pos < 0)
            {
                return text;
            }
            return text.Substring(0, pos) + replace + text.Substring(pos + search.Length);
        }
        //button events
        private void barcodebutton_Click(object sender, EventArgs e)
        {
            numPeople = 0;
            ClearTextBoxes();
            if (barcode.Text != "")
            {
                if (barcode.Text.IndexOf(",") != -1)
                {
                    barcode.Text = barcode.Text.Substring(barcode.Text.IndexOf(",")+1);
                    barcode.Text = barcode.Text.Substring(0, barcode.Text.IndexOf(","));
                    if(barcode.Text.IndexOf('P') == 0)
                    {
                        barcode.Text = barcode.Text.Remove(0, 1);
                    }

                }
                StringBuilder res = new StringBuilder(512);
                PrintReceipt(res, Convert.ToInt32(barcode.Text));
                string str = res.ToString();
                string[] fillString = str.Split(',');
                const int strSize = 22; //const
                numPeople = (fillString.Length / strSize);
                resetPaid();
                for (int i = 0; i < numPeople; i++)
                {
                    switch (i+1)
                    {
                        case 1:
                            //across all
                            paidBox1.ReadOnly = false;
                            checkInBox1.Enabled = true;
                            checkInBox1.Checked = true;
                            keyBox1.Enabled = true;
                            keyBox1.Checked = fillString[21 + i*strSize] == "1" ? true : false;
                            mealBox1.Enabled = true;
                            mealBox1.Checked = fillString[21 + i * strSize] == "1" ? true : false;
                            party_field.Text = fillString[1];
                            contact_field.Text = fillString[3];

                            id_field1.Text = fillString[0 + i*strSize];
                            name1.Text = fillString[5 + i * strSize] + " " + fillString[6 + i * strSize];
                            needRoom1.Text = fillString[16 + i * strSize];
                            ageGroup1.Text = fillString[11 + i * strSize];
                            christian1.Text = fillString[18 + i * strSize];
                            service1.Text = fillString[19 + i * strSize];
                            regfee1.Text = fillString[14 + i * strSize];
                            keyFee1.Text = fillString[15 + i * strSize];
                            if (checkID[Convert.ToInt32(id_field1.Text)]) {
                                status1.Text = "True";
                                status1.BackColor = Color.FromArgb(0, 255, 0);
                            } else { 
                                status1.Text = "False";
                                status1.BackColor = Color.FromArgb(255, 0, 0);
                            }

                            adjustedFee1.Text = Convert.ToString(Convert.ToDouble(keyFee1.Text) + Convert.ToDouble(regfee1.Text));
                            if(paidBox1.Text.Length > 0)
                                adjustedFee1.Text = Convert.ToString(Convert.ToDouble(adjustedFee1.Text) - Convert.ToDouble(paidBox1.Text));
                            partyFee.Text = adjustedFee1.Text;
                            break;
                        case 2:
                            paidBox2.ReadOnly = false;
                            checkInBox2.Enabled = true;
                            checkInBox2.Checked = true;
                            keyBox2.Enabled = true;
                            keyBox2.Checked = fillString[21 + i * strSize] == "1" ? true : false;
                            mealBox2.Enabled = true;
                            mealBox2.Checked = fillString[21 + i * strSize] == "1" ? true : false;

                            id_field2.Text = fillString[0 + i * strSize];
                            name2.Text = fillString[5 + i * strSize] + " " + fillString[6 + i * strSize];
                            needRoom2.Text = fillString[16 + i * strSize];
                            ageGroup2.Text = fillString[11 + i * strSize];
                            christian2.Text = fillString[18 + i * strSize];
                            service2.Text = fillString[19 + i * strSize];
                            regfee2.Text = fillString[14 + i * strSize];
                            keyFee2.Text = fillString[15 + i * strSize];
                            if (checkID[Convert.ToInt32(id_field2.Text)])
                            {
                                status2.Text = "True";
                                status2.BackColor = Color.FromArgb(0, 255, 0);
                            }
                            else
                            {
                                status2.Text = "False";
                                status2.BackColor = Color.FromArgb(255, 0, 0);
                            }

                            adjustedFee2.Text = Convert.ToString(Convert.ToDouble(keyFee2.Text) + Convert.ToDouble(regfee2.Text));
                            if (paidBox2.Text.Length > 0)
                                adjustedFee2.Text = Convert.ToString(Convert.ToDouble(adjustedFee2.Text) - Convert.ToDouble(paidBox2.Text));
                            partyFee.Text = Convert.ToString(Convert.ToDouble(partyFee.Text) + Convert.ToDouble(adjustedFee2.Text));
                            break;
                        case 3:
                            paidBox3.ReadOnly = false;
                            checkInBox3.Enabled = true;
                            checkInBox3.Checked = true;
                            keyBox3.Enabled = true;
                            keyBox3.Checked = fillString[21 + i * strSize] == "1" ? true : false;
                            mealBox3.Enabled = true;
                            mealBox3.Checked = fillString[21 + i * strSize] == "1" ? true : false;

                            id_field3.Text = fillString[0 + i * strSize];
                            name3.Text = fillString[5 + i * strSize] + " " + fillString[6 + i * strSize];
                            needRoom3.Text = fillString[16 + i * strSize];
                            ageGroup3.Text = fillString[11 + i * strSize];
                            christian3.Text = fillString[18 + i * strSize];
                            service3.Text = fillString[19 + i * strSize];
                            regfee3.Text = fillString[14 + i * strSize];
                            keyFee3.Text = fillString[15 + i * strSize];
                            if (checkID[Convert.ToInt32(id_field3.Text)])
                            {
                                status3.Text = "True";
                                status3.BackColor = Color.FromArgb(0, 255, 0);
                            }
                            else
                            {
                                status3.Text = "False";
                                status3.BackColor = Color.FromArgb(255, 0, 0);
                            }

                            adjustedFee3.Text = Convert.ToString(Convert.ToDouble(keyFee3.Text) + Convert.ToDouble(regfee3.Text));
                            if (paidBox3.Text.Length > 0)
                                adjustedFee3.Text = Convert.ToString(Convert.ToDouble(adjustedFee3.Text) - Convert.ToDouble(paidBox3.Text));
                            partyFee.Text = Convert.ToString(Convert.ToDouble(partyFee.Text) + Convert.ToDouble(adjustedFee3.Text));
                            break;
                        case 4:
                            paidBox4.ReadOnly = false;
                            checkInBox4.Enabled = true;
                            checkInBox4.Checked = true;
                            keyBox4.Enabled = true;
                            keyBox4.Checked = fillString[21 + i * strSize] == "1" ? true : false;
                            mealBox4.Enabled = true;
                            mealBox4.Checked = fillString[21 + i * strSize] == "1" ? true : false;

                            id_field4.Text = fillString[0 + i * strSize];
                            name4.Text = fillString[5 + i * strSize] + " " + fillString[6 + i * strSize];
                            needRoom4.Text = fillString[16 + i * strSize];
                            ageGroup4.Text = fillString[11 + i * strSize];
                            christian4.Text = fillString[18 + i * strSize];
                            service4.Text = fillString[19 + i * strSize];
                            regfee4.Text = fillString[14 + i * strSize];
                            keyFee4.Text = fillString[15 + i * strSize];
                            if (checkID[Convert.ToInt32(id_field4.Text)])
                            {
                                status4.Text = "True";
                                status4.BackColor = Color.FromArgb(0, 255, 0);
                            }
                            else
                            {
                                status4.Text = "False";
                                status4.BackColor = Color.FromArgb(255, 0, 0);
                            }

                            adjustedFee4.Text = Convert.ToString(Convert.ToDouble(keyFee4.Text) + Convert.ToDouble(regfee4.Text));
                            if (paidBox4.Text.Length > 0)
                                adjustedFee4.Text = Convert.ToString(Convert.ToDouble(adjustedFee4.Text) - Convert.ToDouble(paidBox4.Text));
                            partyFee.Text = Convert.ToString(Convert.ToDouble(partyFee.Text) + Convert.ToDouble(adjustedFee4.Text));
                            break;
                        case 5:
                            paidBox5.ReadOnly = false;
                            checkInBox5.Enabled = true;
                            checkInBox5.Checked = true;
                            keyBox5.Enabled = true;
                            keyBox5.Checked = fillString[21 + i * strSize] == "1" ? true : false;
                            mealBox5.Enabled = true;
                            mealBox5.Checked = fillString[21 + i * strSize] == "1" ? true : false;

                            id_field5.Text = fillString[0 + i * strSize];
                            name5.Text = fillString[5 + i * strSize] + " " + fillString[6 + i * strSize];
                            needRoom5.Text = fillString[16 + i * strSize];
                            ageGroup5.Text = fillString[11 + i * strSize];
                            christian5.Text = fillString[18 + i * strSize];
                            service5.Text = fillString[19 + i * strSize];
                            regfee5.Text = fillString[14 + i * strSize];
                            keyFee5.Text = fillString[15 + i * strSize];
                            if (checkID[Convert.ToInt32(id_field5.Text)])
                            {
                                status5.Text = "True";
                                status5.BackColor = Color.FromArgb(0, 255, 0);
                            }
                            else
                            {
                                status5.Text = "False";
                                status5.BackColor = Color.FromArgb(255, 0, 0);
                            }

                            adjustedFee5.Text = Convert.ToString(Convert.ToDouble(keyFee5.Text) + Convert.ToDouble(regfee5.Text));
                            if (paidBox5.Text.Length > 0)
                                adjustedFee5.Text = Convert.ToString(Convert.ToDouble(adjustedFee5.Text) - Convert.ToDouble(paidBox5.Text));
                            partyFee.Text = Convert.ToString(Convert.ToDouble(partyFee.Text) + Convert.ToDouble(adjustedFee5.Text));
                            break;
                        case 6:
                            paidBox6.ReadOnly = false;
                            checkInBox6.Enabled = true;
                            checkInBox6.Checked = true;
                            keyBox6.Enabled = true;
                            keyBox6.Checked = fillString[21 + i * strSize] == "1" ? true : false;
                            mealBox6.Enabled = true;
                            mealBox6.Checked = fillString[21 + i * strSize] == "1" ? true : false;

                            id_field6.Text = fillString[0 + i * strSize];
                            name6.Text = fillString[5 + i * strSize] + " " + fillString[6 + i * strSize];
                            needRoom6.Text = fillString[16 + i * strSize];
                            ageGroup6.Text = fillString[11 + i * strSize];
                            christian6.Text = fillString[18 + i * strSize];
                            service6.Text = fillString[19 + i * strSize];
                            regfee6.Text = fillString[14 + i * strSize];
                            keyFee6.Text = fillString[15 + i * strSize];
                            if (checkID[Convert.ToInt32(id_field6.Text)])
                            {
                                status6.Text = "True";
                                status6.BackColor = Color.FromArgb(0, 255, 0);
                            }
                            else
                            {
                                status6.Text = "False";
                                status6.BackColor = Color.FromArgb(255, 0, 0);
                            }

                            adjustedFee6.Text = Convert.ToString(Convert.ToDouble(keyFee6.Text) + Convert.ToDouble(regfee6.Text));
                            if (paidBox6.Text.Length > 0)
                                adjustedFee6.Text = Convert.ToString(Convert.ToDouble(adjustedFee6.Text) - Convert.ToDouble(paidBox6.Text));
                            partyFee.Text = Convert.ToString(Convert.ToDouble(partyFee.Text) + Convert.ToDouble(adjustedFee6.Text));
                            break;
                        case 7:
                            paidBox7.ReadOnly = false;
                            checkInBox7.Enabled = true;
                            checkInBox7.Checked = true;
                            keyBox7.Enabled = true;
                            keyBox7.Checked = fillString[21 + i * strSize] == "1" ? true : false;
                            mealBox7.Enabled = true;
                            mealBox7.Checked = fillString[21 + i * strSize] == "1" ? true : false;

                            id_field7.Text = fillString[0 + i * strSize];
                            name7.Text = fillString[5 + i * strSize] + " " + fillString[6 + i * strSize];
                            needRoom7.Text = fillString[16 + i * strSize];
                            ageGroup7.Text = fillString[11 + i * strSize];
                            christian7.Text = fillString[18 + i * strSize];
                            service7.Text = fillString[19 + i * strSize];
                            regfee7.Text = fillString[14 + i * strSize];
                            keyFee7.Text = fillString[15 + i * strSize];
                            if (checkID[Convert.ToInt32(id_field7.Text)])
                            {
                                status7.Text = "True";
                                status7.BackColor = Color.FromArgb(0, 255, 0);
                            }
                            else
                            {
                                status7.Text = "False";
                                status7.BackColor = Color.FromArgb(255, 0, 0);
                            }

                            adjustedFee7.Text = Convert.ToString(Convert.ToDouble(keyFee7.Text) + Convert.ToDouble(regfee7.Text));
                            if (paidBox7.Text.Length > 0)
                                adjustedFee7.Text = Convert.ToString(Convert.ToDouble(adjustedFee7.Text) - Convert.ToDouble(paidBox7.Text));
                            partyFee.Text = Convert.ToString(Convert.ToDouble(partyFee.Text) + Convert.ToDouble(adjustedFee7.Text));
                            break;
                        default:
                            break;
                    }
                }
                for(int i = 7; i > numPeople; i--)
                {
                    switch(i)
                    {
                        case 1:
                            paidBox1.ReadOnly = true;
                            checkInBox1.Enabled = false;
                            checkInBox1.Checked = false;
                            keyBox1.Enabled = false;
                            keyBox1.Checked = false;
                            mealBox1.Enabled = false;
                            mealBox1.Checked = false;

                            status1.Text = "False";
                            status1.BackColor = Color.FromArgb(255, 0, 0);
                            break;
                        case 2:
                            paidBox2.ReadOnly = true;
                            checkInBox2.Enabled = false;
                            checkInBox2.Checked = false;
                            keyBox2.Enabled = false;
                            keyBox2.Checked = false;
                            mealBox2.Enabled = false;
                            mealBox2.Checked = false;

                            status2.Text = "False";
                            status2.BackColor = Color.FromArgb(255, 0, 0);
                            break;
                        case 3:
                            paidBox3.ReadOnly = true;
                            checkInBox3.Enabled = false;
                            checkInBox3.Checked = false;
                            keyBox3.Enabled = false;
                            keyBox3.Checked = false;
                            mealBox3.Enabled = false;
                            mealBox3.Checked = false;

                            status3.Text = "False";
                            status3.BackColor = Color.FromArgb(255, 0, 0);
                            break;
                        case 4:
                            paidBox4.ReadOnly = true;
                            checkInBox4.Enabled = false;
                            checkInBox4.Checked = false;
                            keyBox4.Enabled = false;
                            keyBox4.Checked = false;
                            mealBox4.Enabled = false;
                            mealBox4.Checked = false;

                            status4.Text = "False";
                            status4.BackColor = Color.FromArgb(255, 0, 0);
                            break;
                        case 5:
                            paidBox5.ReadOnly = true;
                            checkInBox5.Enabled = false;
                            checkInBox5.Checked = false;
                            keyBox5.Enabled = false;
                            keyBox5.Checked = false;
                            mealBox5.Enabled = false;
                            mealBox5.Checked = false;

                            status5.Text = "False";
                            status5.BackColor = Color.FromArgb(255, 0, 0);
                            break;
                        case 6:
                            paidBox6.ReadOnly = true;
                            checkInBox6.Enabled = false;
                            checkInBox6.Checked = false;
                            keyBox6.Enabled = false;
                            keyBox6.Checked = false;
                            mealBox6.Enabled = false;
                            mealBox6.Checked = false;

                            status6.Text = "False";
                            status6.BackColor = Color.FromArgb(255, 0, 0);
                            break;
                        case 7:
                            paidBox7.ReadOnly = true;
                            checkInBox7.Enabled = false;
                            checkInBox7.Checked = false;
                            keyBox7.Enabled = false;
                            keyBox7.Checked = false;
                            mealBox7.Enabled = false;
                            mealBox7.Checked = false;

                            status7.Text = "False";
                            status7.BackColor = Color.FromArgb(255, 0, 0);
                            break;
                        default:
                            break;
                    }
                }
                adjustedDue.Text = partyFee.Text;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //Update the Receipt for Printing
            string receipt = System.IO.File.ReadAllText(Environment.CurrentDirectory + @"\qrres\receipt_template.html");
            for (int i = 0; i < numPeople; i++)
            {
                switch (i+1)
                {
                    case 1:
                        receipt = ReplaceFirst(receipt, "KEYSTATUS_FIELD", keyBox1.Checked ? "Yes _________" : "No");
                        receipt = ReplaceFirst(receipt, "CARDSTATUS_FIELD", mealBox1.Checked ? "Yes _________" : "No");
                        break;
                    case 2:
                        receipt = ReplaceFirst(receipt, "KEYSTATUS_FIELD", keyBox2.Checked ? "Yes _________" : "No");
                        receipt = ReplaceFirst(receipt, "CARDSTATUS_FIELD", mealBox2.Checked ? "Yes _________" : "No");
                        break;
                    case 3:
                        receipt = ReplaceFirst(receipt, "KEYSTATUS_FIELD", keyBox3.Checked ? "Yes _________" : "No");
                        receipt = ReplaceFirst(receipt, "CARDSTATUS_FIELD", mealBox3.Checked ? "Yes _________" : "No");
                        break;
                    case 4:
                        receipt = ReplaceFirst(receipt, "KEYSTATUS_FIELD", keyBox4.Checked ? "Yes _________" : "No");
                        receipt = ReplaceFirst(receipt, "CARDSTATUS_FIELD", mealBox4.Checked ? "Yes _________" : "No");
                        break;
                    case 5:
                        receipt = ReplaceFirst(receipt, "KEYSTATUS_FIELD", keyBox5.Checked ? "Yes _________" : "No");
                        receipt = ReplaceFirst(receipt, "CARDSTATUS_FIELD", mealBox5.Checked ? "Yes _________" : "No");
                        break;
                    case 6:
                        receipt = ReplaceFirst(receipt, "KEYSTATUS_FIELD", keyBox6.Checked ? "Yes _________" : "No");
                        receipt = ReplaceFirst(receipt, "CARDSTATUS_FIELD", mealBox6.Checked ? "Yes _________" : "No");
                        break;
                    case 7:
                        receipt = ReplaceFirst(receipt, "KEYSTATUS_FIELD", keyBox7.Checked ? "Yes _________" : "No");
                        receipt = ReplaceFirst(receipt, "CARDSTATUS_FIELD", mealBox7.Checked ? "Yes _________" : "No");
                        break;
                    default:
                        break;

                }
            }
            using (System.IO.StreamWriter file =
            new System.IO.StreamWriter(Environment.CurrentDirectory + @"\qrres\receipt_template.html", false)) {
                file.WriteLine(receipt);
            }
            WebBrowser wBForPrinting = new WebBrowser();
            wBForPrinting.DocumentCompleted += new WebBrowserDocumentCompletedEventHandler(PrintDocument);
            wBForPrinting.Url = new Uri(Environment.CurrentDirectory + @"\qrres\receipt_template.html");
        }

        private void button2_Click(object sender, EventArgs e)
        {
            string data;
            using (System.IO.StreamWriter file =
            new System.IO.StreamWriter(Environment.CurrentDirectory + @"\qrres\qfl_checkedin-data.csv", true))
            {
                for (int i = 0; i < numPeople; i++)
                {
                    switch (i + 1)
                    {
                        case 1:
                            if (checkInBox1.Checked)
                            {
                                data = id_field1.Text + "," +
                                    party_field.Text + "," +
                                    name1.Text + "," +
                                    regfee1.Text + "," +
                                    keyFee1.Text + "," +
                                    keyBox1.Checked + "," +
                                    mealBox1.Checked;
                                file.WriteLine(data);
                                checkID[Convert.ToInt32(id_field1.Text)] = true;
                                status1.Text = "Yes";
                                status1.BackColor = Color.FromArgb(0, 255, 0);
                            }
                            break;
                        case 2:
                            if (checkInBox2.Checked)
                            {
                                data = id_field2.Text + "," +
                                    party_field.Text + "," +
                                    name2.Text + "," +
                                    regfee2.Text + "," +
                                    keyFee2.Text + "," +
                                    keyBox2.Checked + "," +
                                    mealBox2.Checked;
                                file.WriteLine(data);
                                checkID[Convert.ToInt32(id_field2.Text)] = true;
                                status2.Text = "Yes";
                                status2.BackColor = Color.FromArgb(0, 255, 0);
                            }
                            break;
                        case 3:
                            if (checkInBox3.Checked)
                            {
                                data = id_field3.Text + "," +
                                    party_field.Text + "," +
                                    name3.Text + "," +
                                    regfee3.Text + "," +
                                    keyFee3.Text + "," +
                                    keyBox3.Checked + "," +
                                    mealBox3.Checked;
                                file.WriteLine(data);
                                checkID[Convert.ToInt32(id_field3.Text)] = true;
                                status3.Text = "Yes";
                                status3.BackColor = Color.FromArgb(0, 255, 0);
                            }
                            break;
                        case 4:
                            if (checkInBox4.Checked)
                            {
                                data = id_field4.Text + "," +
                                    party_field.Text + "," +
                                    name4.Text + "," +
                                    regfee4.Text + "," +
                                    keyFee4.Text + "," +
                                    keyBox4.Checked + "," +
                                    mealBox4.Checked;
                                file.WriteLine(data);
                                checkID[Convert.ToInt32(id_field4.Text)] = true;
                                status4.Text = "Yes";
                                status4.BackColor = Color.FromArgb(0, 255, 0);
                            }
                            break;
                        case 5:
                            if (checkInBox5.Checked)
                            {
                                data = id_field5.Text + "," +
                                    party_field.Text + "," +
                                    name5.Text + "," +
                                    regfee5.Text + "," +
                                    keyFee5.Text + "," +
                                    keyBox5.Checked + "," +
                                    mealBox5.Checked;
                                file.WriteLine(data);
                                checkID[Convert.ToInt32(id_field5.Text)] = true;
                                status5.Text = "Yes";
                                status5.BackColor = Color.FromArgb(0, 255, 0);
                            }
                            break;
                        case 6:
                            if (checkInBox6.Checked)
                            {
                                data = id_field6.Text + "," +
                                    party_field.Text + "," +
                                    name6.Text + "," +
                                    regfee6.Text + "," +
                                    keyFee6.Text + "," +
                                    keyBox6.Checked + "," +
                                    mealBox6.Checked;
                                file.WriteLine(data);
                                checkID[Convert.ToInt32(id_field6.Text)] = true;
                                status6.Text = "Yes";
                                status6.BackColor = Color.FromArgb(0, 255, 0);
                            }
                            break;
                        case 7:
                            if (checkInBox7.Checked)
                            {
                                data = id_field7.Text + "," +
                                    party_field.Text + "," +
                                    name7.Text + "," +
                                    regfee7.Text + "," +
                                    keyFee7.Text + "," +
                                    keyBox7.Checked + "," +
                                    mealBox7.Checked;
                                file.WriteLine(data);
                                checkID[Convert.ToInt32(id_field7.Text)] = true;
                                status7.Text = "Yes";
                                status7.BackColor = Color.FromArgb(0, 255, 0);
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
        }

        private void partyPaid_TextChanged(object sender, EventArgs e)
        {
            adjustDues();
        }

        private void paidBox1_TextChanged(object sender, EventArgs e)
        {
            adjustedFee1.Text = Convert.ToString(Convert.ToDouble(keyFee1.Text) + Convert.ToDouble(regfee1.Text));
            if (paidBox1.Text.Length > 0)
                adjustedFee1.Text = Convert.ToString(Convert.ToDouble(adjustedFee1.Text) - Convert.ToDouble(paidBox1.Text));
            adjustDues();
        }

        private void paidBox2_TextChanged(object sender, EventArgs e)
        {
            adjustedFee2.Text = Convert.ToString(Convert.ToDouble(keyFee2.Text) + Convert.ToDouble(regfee2.Text));
            if (paidBox2.Text.Length > 0)
                adjustedFee2.Text = Convert.ToString(Convert.ToDouble(adjustedFee2.Text) - Convert.ToDouble(paidBox2.Text));
            adjustDues();
        }

        private void paidBox3_TextChanged(object sender, EventArgs e)
        {
            adjustedFee3.Text = Convert.ToString(Convert.ToDouble(keyFee3.Text) + Convert.ToDouble(regfee3.Text));
            if (paidBox3.Text.Length > 0)
                adjustedFee3.Text = Convert.ToString(Convert.ToDouble(adjustedFee3.Text) - Convert.ToDouble(paidBox3.Text));
            adjustDues();
        }

        private void paidBox4_TextChanged(object sender, EventArgs e)
        {
            adjustedFee4.Text = Convert.ToString(Convert.ToDouble(keyFee4.Text) + Convert.ToDouble(regfee4.Text));
            if (paidBox4.Text.Length > 0)
                adjustedFee4.Text = Convert.ToString(Convert.ToDouble(adjustedFee4.Text) - Convert.ToDouble(paidBox4.Text));
            adjustDues();
        }

        private void paidBox5_TextChanged(object sender, EventArgs e)
        {
            adjustedFee5.Text = Convert.ToString(Convert.ToDouble(keyFee5.Text) + Convert.ToDouble(regfee5.Text));
            if (paidBox5.Text.Length > 0)
                adjustedFee5.Text = Convert.ToString(Convert.ToDouble(adjustedFee5.Text) - Convert.ToDouble(paidBox5.Text));
            adjustDues();
        }

        private void paidBox6_TextChanged(object sender, EventArgs e)
        {
            adjustedFee6.Text = Convert.ToString(Convert.ToDouble(keyFee6.Text) + Convert.ToDouble(regfee6.Text));
            if (paidBox6.Text.Length > 0)
                adjustedFee6.Text = Convert.ToString(Convert.ToDouble(adjustedFee6.Text) - Convert.ToDouble(paidBox6.Text));
            adjustDues();
        }

        private void paidBox7_TextChanged(object sender, EventArgs e)
        {
            adjustedFee7.Text = Convert.ToString(Convert.ToDouble(keyFee7.Text) + Convert.ToDouble(regfee7.Text));
            if (paidBox7.Text.Length > 0)
                adjustedFee7.Text = Convert.ToString(Convert.ToDouble(adjustedFee7.Text) - Convert.ToDouble(paidBox7.Text));
            adjustDues();
        }

        private void mealBox1_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void mealBox2_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void mealBox3_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void mealBox4_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void mealBox5_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void mealBox6_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void mealBox7_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void payNote_TextChanged(object sender, EventArgs e)
        {

        }

        private void contact_field_TextChanged(object sender, EventArgs e)
        {

        }

        private void label22_Click(object sender, EventArgs e)
        {

        }

        private void label23_Click(object sender, EventArgs e)
        {

        }

        private void partyFee_TextChanged(object sender, EventArgs e)
        {

        }

        private void adjustedDue_TextChanged(object sender, EventArgs e)
        {

        }

        private void label29_Click(object sender, EventArgs e)
        {

        }
    }
}
