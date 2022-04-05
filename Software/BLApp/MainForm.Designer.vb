<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class MainForm
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.Panel2 = New System.Windows.Forms.Panel()
        Me.TestGroup = New System.Windows.Forms.GroupBox()
        Me.Button9 = New System.Windows.Forms.Button()
        Me.TableLayoutPanel1 = New System.Windows.Forms.TableLayoutPanel()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.ExportLogCmd = New System.Windows.Forms.Button()
        Me.ClearLogCmd = New System.Windows.Forms.Button()
        Me.ConnectDisconnectCmd = New System.Windows.Forms.Button()
        Me.PortScanCmd = New System.Windows.Forms.Button()
        Me.COMCombo = New System.Windows.Forms.ComboBox()
        Me.TableLayoutPanel2 = New System.Windows.Forms.TableLayoutPanel()
        Me.LogTextBox = New System.Windows.Forms.TextBox()
        Me.TableLayoutPanel3 = New System.Windows.Forms.TableLayoutPanel()
        Me.Panel1 = New System.Windows.Forms.Panel()
        Me.ProgressBar1 = New System.Windows.Forms.ProgressBar()
        Me.TestGroup.SuspendLayout()
        Me.TableLayoutPanel1.SuspendLayout()
        Me.GroupBox1.SuspendLayout()
        Me.TableLayoutPanel2.SuspendLayout()
        Me.TableLayoutPanel3.SuspendLayout()
        Me.Panel1.SuspendLayout()
        Me.SuspendLayout()
        '
        'Panel2
        '
        Me.Panel2.AutoScroll = True
        Me.Panel2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel2.Location = New System.Drawing.Point(396, 3)
        Me.Panel2.Name = "Panel2"
        Me.Panel2.Size = New System.Drawing.Size(585, 405)
        Me.Panel2.TabIndex = 1
        '
        'TestGroup
        '
        Me.TestGroup.Controls.Add(Me.ProgressBar1)
        Me.TestGroup.Controls.Add(Me.Button9)
        Me.TestGroup.Dock = System.Windows.Forms.DockStyle.Top
        Me.TestGroup.Enabled = False
        Me.TestGroup.Location = New System.Drawing.Point(0, 0)
        Me.TestGroup.Name = "TestGroup"
        Me.TestGroup.Size = New System.Drawing.Size(387, 128)
        Me.TestGroup.TabIndex = 0
        Me.TestGroup.TabStop = False
        Me.TestGroup.Text = "API Test bench"
        '
        'Button9
        '
        Me.Button9.Location = New System.Drawing.Point(6, 29)
        Me.Button9.Name = "Button9"
        Me.Button9.Size = New System.Drawing.Size(96, 27)
        Me.Button9.TabIndex = 8
        Me.Button9.Text = "Firmware update"
        Me.Button9.UseVisualStyleBackColor = True
        '
        'TableLayoutPanel1
        '
        Me.TableLayoutPanel1.ColumnCount = 1
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.TableLayoutPanel1.Controls.Add(Me.GroupBox1, 0, 0)
        Me.TableLayoutPanel1.Controls.Add(Me.TableLayoutPanel2, 0, 1)
        Me.TableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TableLayoutPanel1.Location = New System.Drawing.Point(0, 0)
        Me.TableLayoutPanel1.Name = "TableLayoutPanel1"
        Me.TableLayoutPanel1.RowCount = 2
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 9.715994!))
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 90.284!))
        Me.TableLayoutPanel1.Size = New System.Drawing.Size(996, 662)
        Me.TableLayoutPanel1.TabIndex = 1
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.ExportLogCmd)
        Me.GroupBox1.Controls.Add(Me.ClearLogCmd)
        Me.GroupBox1.Controls.Add(Me.ConnectDisconnectCmd)
        Me.GroupBox1.Controls.Add(Me.PortScanCmd)
        Me.GroupBox1.Controls.Add(Me.COMCombo)
        Me.GroupBox1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.GroupBox1.Location = New System.Drawing.Point(3, 3)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(990, 58)
        Me.GroupBox1.TabIndex = 2
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "Connection Setup"
        '
        'ExportLogCmd
        '
        Me.ExportLogCmd.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.ExportLogCmd.Location = New System.Drawing.Point(746, 17)
        Me.ExportLogCmd.Name = "ExportLogCmd"
        Me.ExportLogCmd.Size = New System.Drawing.Size(117, 28)
        Me.ExportLogCmd.TabIndex = 5
        Me.ExportLogCmd.Text = "Export log"
        Me.ExportLogCmd.UseVisualStyleBackColor = True
        Me.ExportLogCmd.Visible = False
        '
        'ClearLogCmd
        '
        Me.ClearLogCmd.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.ClearLogCmd.Location = New System.Drawing.Point(866, 17)
        Me.ClearLogCmd.Name = "ClearLogCmd"
        Me.ClearLogCmd.Size = New System.Drawing.Size(117, 28)
        Me.ClearLogCmd.TabIndex = 4
        Me.ClearLogCmd.Text = "Clear log text"
        Me.ClearLogCmd.UseVisualStyleBackColor = True
        '
        'ConnectDisconnectCmd
        '
        Me.ConnectDisconnectCmd.Location = New System.Drawing.Point(280, 19)
        Me.ConnectDisconnectCmd.Name = "ConnectDisconnectCmd"
        Me.ConnectDisconnectCmd.Size = New System.Drawing.Size(117, 28)
        Me.ConnectDisconnectCmd.TabIndex = 3
        Me.ConnectDisconnectCmd.Text = "Connect"
        Me.ConnectDisconnectCmd.UseVisualStyleBackColor = True
        '
        'PortScanCmd
        '
        Me.PortScanCmd.Location = New System.Drawing.Point(144, 19)
        Me.PortScanCmd.Name = "PortScanCmd"
        Me.PortScanCmd.Size = New System.Drawing.Size(117, 28)
        Me.PortScanCmd.TabIndex = 2
        Me.PortScanCmd.Text = "Rescan"
        Me.PortScanCmd.UseVisualStyleBackColor = True
        '
        'COMCombo
        '
        Me.COMCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.COMCombo.FormattingEnabled = True
        Me.COMCombo.Location = New System.Drawing.Point(9, 24)
        Me.COMCombo.Name = "COMCombo"
        Me.COMCombo.Size = New System.Drawing.Size(114, 21)
        Me.COMCombo.TabIndex = 1
        '
        'TableLayoutPanel2
        '
        Me.TableLayoutPanel2.ColumnCount = 1
        Me.TableLayoutPanel2.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 48.23411!))
        Me.TableLayoutPanel2.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 51.76589!))
        Me.TableLayoutPanel2.Controls.Add(Me.LogTextBox, 0, 1)
        Me.TableLayoutPanel2.Controls.Add(Me.TableLayoutPanel3, 0, 0)
        Me.TableLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TableLayoutPanel2.Location = New System.Drawing.Point(3, 67)
        Me.TableLayoutPanel2.Name = "TableLayoutPanel2"
        Me.TableLayoutPanel2.RowCount = 2
        Me.TableLayoutPanel2.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 70.58823!))
        Me.TableLayoutPanel2.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 29.41176!))
        Me.TableLayoutPanel2.Size = New System.Drawing.Size(990, 592)
        Me.TableLayoutPanel2.TabIndex = 3
        '
        'LogTextBox
        '
        Me.LogTextBox.Dock = System.Windows.Forms.DockStyle.Fill
        Me.LogTextBox.Font = New System.Drawing.Font("Courier New", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.LogTextBox.Location = New System.Drawing.Point(3, 420)
        Me.LogTextBox.MaxLength = 10383360
        Me.LogTextBox.Multiline = True
        Me.LogTextBox.Name = "LogTextBox"
        Me.LogTextBox.ReadOnly = True
        Me.LogTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.LogTextBox.Size = New System.Drawing.Size(984, 169)
        Me.LogTextBox.TabIndex = 0
        Me.LogTextBox.Text = "[Log]" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10)
        '
        'TableLayoutPanel3
        '
        Me.TableLayoutPanel3.ColumnCount = 2
        Me.TableLayoutPanel3.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 40.0!))
        Me.TableLayoutPanel3.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 60.0!))
        Me.TableLayoutPanel3.Controls.Add(Me.Panel1, 0, 0)
        Me.TableLayoutPanel3.Controls.Add(Me.Panel2, 1, 0)
        Me.TableLayoutPanel3.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TableLayoutPanel3.Location = New System.Drawing.Point(3, 3)
        Me.TableLayoutPanel3.Name = "TableLayoutPanel3"
        Me.TableLayoutPanel3.RowCount = 1
        Me.TableLayoutPanel3.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel3.Size = New System.Drawing.Size(984, 411)
        Me.TableLayoutPanel3.TabIndex = 1
        '
        'Panel1
        '
        Me.Panel1.AutoScroll = True
        Me.Panel1.Controls.Add(Me.TestGroup)
        Me.Panel1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel1.Location = New System.Drawing.Point(3, 3)
        Me.Panel1.Name = "Panel1"
        Me.Panel1.Size = New System.Drawing.Size(387, 405)
        Me.Panel1.TabIndex = 0
        '
        'ProgressBar1
        '
        Me.ProgressBar1.Anchor = CType(((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.ProgressBar1.Location = New System.Drawing.Point(6, 103)
        Me.ProgressBar1.Name = "ProgressBar1"
        Me.ProgressBar1.Size = New System.Drawing.Size(375, 19)
        Me.ProgressBar1.TabIndex = 9
        '
        'MainForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(996, 662)
        Me.Controls.Add(Me.TableLayoutPanel1)
        Me.Name = "MainForm"
        Me.Text = "Bootloader"
        Me.TestGroup.ResumeLayout(False)
        Me.TableLayoutPanel1.ResumeLayout(False)
        Me.GroupBox1.ResumeLayout(False)
        Me.TableLayoutPanel2.ResumeLayout(False)
        Me.TableLayoutPanel2.PerformLayout()
        Me.TableLayoutPanel3.ResumeLayout(False)
        Me.Panel1.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents Panel2 As Panel
    Friend WithEvents TestGroup As GroupBox
    Friend WithEvents TableLayoutPanel1 As TableLayoutPanel
    Friend WithEvents GroupBox1 As GroupBox
    Friend WithEvents ExportLogCmd As Button
    Friend WithEvents ClearLogCmd As Button
    Friend WithEvents ConnectDisconnectCmd As Button
    Friend WithEvents PortScanCmd As Button
    Friend WithEvents COMCombo As ComboBox
    Friend WithEvents TableLayoutPanel2 As TableLayoutPanel
    Friend WithEvents LogTextBox As TextBox
    Friend WithEvents TableLayoutPanel3 As TableLayoutPanel
    Friend WithEvents Panel1 As Panel
    Friend WithEvents Button9 As Button
    Friend WithEvents ProgressBar1 As ProgressBar
End Class
