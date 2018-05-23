namespace qflprinter_gui
{
    partial class PrintForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.print_button = new System.Windows.Forms.Button();
            this.ChineseN = new System.Windows.Forms.Label();
            this.cname = new System.Windows.Forms.TextBox();
            this.lname = new System.Windows.Forms.TextBox();
            this.fname = new System.Windows.Forms.TextBox();
            this.LastN = new System.Windows.Forms.Label();
            this.FirstN = new System.Windows.Forms.Label();
            this.pid_field = new System.Windows.Forms.TextBox();
            this.id_field = new System.Windows.Forms.TextBox();
            this.pid = new System.Windows.Forms.Label();
            this.id = new System.Windows.Forms.Label();
            this.group_field = new System.Windows.Forms.TextBox();
            this.group = new System.Windows.Forms.Label();
            this.church_field = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.load = new System.Windows.Forms.Button();
            this.loadid = new System.Windows.Forms.TextBox();
            this.idloader = new System.Windows.Forms.Label();
            this.gender = new System.Windows.Forms.TextBox();
            this.age = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.needRoom = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.grade = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Arial Rounded MT Bold", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(45, 38);
            this.label1.Margin = new System.Windows.Forms.Padding(8, 0, 8, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(828, 93);
            this.label1.TabIndex = 7;
            this.label1.Text = "QFL Print Name-Tag";
            this.label1.Click += new System.EventHandler(this.label1_Click);
            // 
            // print_button
            // 
            this.print_button.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.print_button.Location = new System.Drawing.Point(64, 1226);
            this.print_button.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.print_button.Name = "print_button";
            this.print_button.Size = new System.Drawing.Size(723, 138);
            this.print_button.TabIndex = 6;
            this.print_button.Text = "Print";
            this.print_button.UseVisualStyleBackColor = true;
            this.print_button.Click += new System.EventHandler(this.button1_Click);
            // 
            // ChineseN
            // 
            this.ChineseN.AutoSize = true;
            this.ChineseN.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ChineseN.Location = new System.Drawing.Point(755, 565);
            this.ChineseN.Margin = new System.Windows.Forms.Padding(8, 0, 8, 0);
            this.ChineseN.Name = "ChineseN";
            this.ChineseN.Size = new System.Drawing.Size(390, 61);
            this.ChineseN.TabIndex = 17;
            this.ChineseN.Text = "Chinese Name:";
            // 
            // cname
            // 
            this.cname.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cname.Location = new System.Drawing.Point(768, 632);
            this.cname.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.cname.Name = "cname";
            this.cname.ReadOnly = true;
            this.cname.Size = new System.Drawing.Size(543, 67);
            this.cname.TabIndex = 16;
            this.cname.TextChanged += new System.EventHandler(this.cname_TextChanged);
            // 
            // lname
            // 
            this.lname.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lname.Location = new System.Drawing.Point(768, 429);
            this.lname.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.lname.Name = "lname";
            this.lname.ReadOnly = true;
            this.lname.Size = new System.Drawing.Size(543, 67);
            this.lname.TabIndex = 15;
            // 
            // fname
            // 
            this.fname.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.fname.Location = new System.Drawing.Point(768, 229);
            this.fname.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.fname.Name = "fname";
            this.fname.ReadOnly = true;
            this.fname.Size = new System.Drawing.Size(543, 67);
            this.fname.TabIndex = 14;
            this.fname.TextChanged += new System.EventHandler(this.fname_TextChanged);
            // 
            // LastN
            // 
            this.LastN.AutoSize = true;
            this.LastN.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.LastN.Location = new System.Drawing.Point(752, 372);
            this.LastN.Margin = new System.Windows.Forms.Padding(8, 0, 8, 0);
            this.LastN.Name = "LastN";
            this.LastN.Size = new System.Drawing.Size(296, 61);
            this.LastN.TabIndex = 13;
            this.LastN.Text = "Last Name:";
            // 
            // FirstN
            // 
            this.FirstN.AutoSize = true;
            this.FirstN.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FirstN.Location = new System.Drawing.Point(755, 162);
            this.FirstN.Margin = new System.Windows.Forms.Padding(8, 0, 8, 0);
            this.FirstN.Name = "FirstN";
            this.FirstN.Size = new System.Drawing.Size(301, 61);
            this.FirstN.TabIndex = 12;
            this.FirstN.Text = "First Name:";
            this.FirstN.Click += new System.EventHandler(this.FirstN_Click);
            // 
            // pid_field
            // 
            this.pid_field.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.pid_field.Location = new System.Drawing.Point(61, 429);
            this.pid_field.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.pid_field.Name = "pid_field";
            this.pid_field.ReadOnly = true;
            this.pid_field.Size = new System.Drawing.Size(553, 67);
            this.pid_field.TabIndex = 21;
            this.pid_field.TextChanged += new System.EventHandler(this.textBox1_TextChanged);
            // 
            // id_field
            // 
            this.id_field.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.id_field.Location = new System.Drawing.Point(64, 229);
            this.id_field.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.id_field.Name = "id_field";
            this.id_field.ReadOnly = true;
            this.id_field.Size = new System.Drawing.Size(553, 67);
            this.id_field.TabIndex = 20;
            this.id_field.TextChanged += new System.EventHandler(this.textBox2_TextChanged);
            // 
            // pid
            // 
            this.pid.AutoSize = true;
            this.pid.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.pid.Location = new System.Drawing.Point(51, 365);
            this.pid.Margin = new System.Windows.Forms.Padding(8, 0, 8, 0);
            this.pid.Name = "pid";
            this.pid.Size = new System.Drawing.Size(434, 61);
            this.pid.TabIndex = 19;
            this.pid.Text = "Party ID Number:";
            this.pid.Click += new System.EventHandler(this.label2_Click);
            // 
            // id
            // 
            this.id.AutoSize = true;
            this.id.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.id.Location = new System.Drawing.Point(48, 162);
            this.id.Margin = new System.Windows.Forms.Padding(8, 0, 8, 0);
            this.id.Name = "id";
            this.id.Size = new System.Drawing.Size(296, 61);
            this.id.TabIndex = 18;
            this.id.Text = "ID Number:";
            this.id.Click += new System.EventHandler(this.label3_Click);
            // 
            // group_field
            // 
            this.group_field.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.group_field.Location = new System.Drawing.Point(64, 632);
            this.group_field.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.group_field.Name = "group_field";
            this.group_field.ReadOnly = true;
            this.group_field.Size = new System.Drawing.Size(553, 67);
            this.group_field.TabIndex = 23;
            // 
            // group
            // 
            this.group.AutoSize = true;
            this.group.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.group.Location = new System.Drawing.Point(51, 565);
            this.group.Margin = new System.Windows.Forms.Padding(8, 0, 8, 0);
            this.group.Name = "group";
            this.group.Size = new System.Drawing.Size(374, 61);
            this.group.TabIndex = 22;
            this.group.Text = "Group Number";
            // 
            // church_field
            // 
            this.church_field.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.church_field.Location = new System.Drawing.Point(72, 863);
            this.church_field.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.church_field.Name = "church_field";
            this.church_field.ReadOnly = true;
            this.church_field.Size = new System.Drawing.Size(1239, 67);
            this.church_field.TabIndex = 25;
            this.church_field.TextChanged += new System.EventHandler(this.textBox1_TextChanged_1);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(59, 796);
            this.label2.Margin = new System.Windows.Forms.Padding(8, 0, 8, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(366, 61);
            this.label2.TabIndex = 24;
            this.label2.Text = "Church Name:";
            this.label2.Click += new System.EventHandler(this.label2_Click_1);
            // 
            // load
            // 
            this.load.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.load.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.load.Location = new System.Drawing.Point(1624, 1226);
            this.load.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.load.Name = "load";
            this.load.Size = new System.Drawing.Size(379, 138);
            this.load.TabIndex = 26;
            this.load.Text = "Load ID";
            this.load.UseVisualStyleBackColor = true;
            this.load.Click += new System.EventHandler(this.load_Click);
            // 
            // loadid
            // 
            this.loadid.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.loadid.Location = new System.Drawing.Point(1133, 1290);
            this.loadid.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.loadid.Name = "loadid";
            this.loadid.Size = new System.Drawing.Size(441, 67);
            this.loadid.TabIndex = 27;
            this.loadid.TextChanged += new System.EventHandler(this.loadid_TextChanged);
            // 
            // idloader
            // 
            this.idloader.AutoSize = true;
            this.idloader.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.idloader.Location = new System.Drawing.Point(1123, 1226);
            this.idloader.Margin = new System.Windows.Forms.Padding(8, 0, 8, 0);
            this.idloader.Name = "idloader";
            this.idloader.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.idloader.Size = new System.Drawing.Size(402, 55);
            this.idloader.TabIndex = 28;
            this.idloader.Text = "QR Code Loader:";
            this.idloader.UseWaitCursor = true;
            this.idloader.Click += new System.EventHandler(this.label3_Click_1);
            // 
            // gender
            // 
            this.gender.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.gender.Location = new System.Drawing.Point(1461, 229);
            this.gender.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.gender.Name = "gender";
            this.gender.ReadOnly = true;
            this.gender.Size = new System.Drawing.Size(535, 67);
            this.gender.TabIndex = 29;
            // 
            // age
            // 
            this.age.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.age.Location = new System.Drawing.Point(1461, 429);
            this.age.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.age.Name = "age";
            this.age.ReadOnly = true;
            this.age.Size = new System.Drawing.Size(535, 67);
            this.age.TabIndex = 30;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(1448, 162);
            this.label3.Margin = new System.Windows.Forms.Padding(8, 0, 8, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(217, 61);
            this.label3.TabIndex = 31;
            this.label3.Text = "Gender:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(1448, 362);
            this.label4.Margin = new System.Windows.Forms.Padding(8, 0, 8, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(149, 61);
            this.label4.TabIndex = 32;
            this.label4.Text = "Age: ";
            this.label4.Click += new System.EventHandler(this.label4_Click);
            // 
            // needRoom
            // 
            this.needRoom.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.needRoom.Location = new System.Drawing.Point(1461, 863);
            this.needRoom.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.needRoom.Name = "needRoom";
            this.needRoom.ReadOnly = true;
            this.needRoom.Size = new System.Drawing.Size(535, 67);
            this.needRoom.TabIndex = 33;
            this.needRoom.TextChanged += new System.EventHandler(this.needRoom_TextChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(1448, 796);
            this.label5.Margin = new System.Windows.Forms.Padding(8, 0, 8, 0);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(379, 61);
            this.label5.TabIndex = 37;
            this.label5.Text = "Room Needed:";
            this.label5.Click += new System.EventHandler(this.label5_Click);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(1448, 565);
            this.label6.Margin = new System.Windows.Forms.Padding(8, 0, 8, 0);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(202, 61);
            this.label6.TabIndex = 39;
            this.label6.Text = "Grade: ";
            this.label6.Click += new System.EventHandler(this.label6_Click_1);
            // 
            // grade
            // 
            this.grade.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.grade.Location = new System.Drawing.Point(1461, 632);
            this.grade.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.grade.Name = "grade";
            this.grade.ReadOnly = true;
            this.grade.Size = new System.Drawing.Size(535, 67);
            this.grade.TabIndex = 38;
            this.grade.TextChanged += new System.EventHandler(this.textBox1_TextChanged_2);
            // 
            // PrintForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(16F, 31F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.ClientSize = new System.Drawing.Size(2096, 1393);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.grade);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.needRoom);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.age);
            this.Controls.Add(this.gender);
            this.Controls.Add(this.idloader);
            this.Controls.Add(this.loadid);
            this.Controls.Add(this.load);
            this.Controls.Add(this.church_field);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.group_field);
            this.Controls.Add(this.group);
            this.Controls.Add(this.pid_field);
            this.Controls.Add(this.id_field);
            this.Controls.Add(this.pid);
            this.Controls.Add(this.id);
            this.Controls.Add(this.ChineseN);
            this.Controls.Add(this.cname);
            this.Controls.Add(this.lname);
            this.Controls.Add(this.fname);
            this.Controls.Add(this.LastN);
            this.Controls.Add(this.FirstN);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.print_button);
            this.Margin = new System.Windows.Forms.Padding(8, 7, 8, 7);
            this.Name = "PrintForm";
            this.Text = "QFL Print Tag Form";
            this.Load += new System.EventHandler(this.PrintForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button print_button;
        private System.Windows.Forms.Label ChineseN;
        private System.Windows.Forms.TextBox cname;
        private System.Windows.Forms.TextBox lname;
        private System.Windows.Forms.TextBox fname;
        private System.Windows.Forms.Label LastN;
        private System.Windows.Forms.Label FirstN;
        private System.Windows.Forms.TextBox pid_field;
        private System.Windows.Forms.TextBox id_field;
        private System.Windows.Forms.Label pid;
        private System.Windows.Forms.Label id;
        private System.Windows.Forms.TextBox group_field;
        private System.Windows.Forms.Label group;
        private System.Windows.Forms.TextBox church_field;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button load;
        private System.Windows.Forms.TextBox loadid;
        private System.Windows.Forms.Label idloader;
        private System.Windows.Forms.TextBox gender;
        private System.Windows.Forms.TextBox age;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox needRoom;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox grade;
    }
}