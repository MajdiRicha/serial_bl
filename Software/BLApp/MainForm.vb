Public Class MainForm

    WithEvents BLCtrl As New BLLib.BLCtrl

    ''' <summary>
    ''' 
    ''' </summary>
    Private Sub ScanPorts()
        Try
            COMCombo.Items.Clear()
            For Each sp As String In My.Computer.Ports.SerialPortNames
                COMCombo.Items.Add(sp)
            Next
            If COMCombo.Items.Count > 0 Then COMCombo.SelectedIndex = 0
        Catch ex As Exception

        End Try
    End Sub


    Private Sub MainForm_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        If SystemInformation.PowerStatus.PowerLineStatus = PowerLineStatus.Offline Then
            MsgBox("For a better performance, keep your computer on AC power.", MsgBoxStyle.Information)
        End If

        Me.Text = Me.Text + " - Rev: " + Me.GetType.Assembly.GetName.Version.Major.ToString() + "." + Me.GetType.Assembly.GetName.Version.Minor.ToString() + "." _
                       + Me.GetType.Assembly.GetName.Version.Build.ToString() + "." + Me.GetType.Assembly.GetName.Version.Revision.ToString()

    End Sub

    Private Sub PortScanCmd_Click(sender As Object, e As EventArgs) Handles PortScanCmd.Click
        Try
            LogTextBox.AppendText("Scanning ports... ")
            ConnectDisconnectCmd.Enabled = False

            ScanPorts()
            ConnectDisconnectCmd.Enabled = True
            LogTextBox.AppendText("Ok" + vbCrLf)
        Catch ex As Exception

        End Try
    End Sub

    Private Sub ConnectDisconnectCmd_Click(sender As Object, e As EventArgs) Handles ConnectDisconnectCmd.Click
        Dim Rev As String = ""

        If ConnectDisconnectCmd.Text = "Connect" Then
            LogTextBox.AppendText("Connecting... ")
            If BLCtrl.Connect(COMCombo.Text) Then

                ConnectDisconnectCmd.Text = "Disconnect"
                LogTextBox.AppendText("Ok" + vbCrLf)
                'Get revision info:
                BLCtrl.GetFirmwareRevision(Rev)
                LogTextBox.AppendText("Firmware rev: " + Rev + vbCrLf)
                BLCtrl.GetFirmwareDate(Rev)
                LogTextBox.AppendText("Firmware cdt: " + Rev + vbCrLf)
                TestGroup.Enabled = True
            Else
                LogTextBox.AppendText("ERROR" + vbCrLf)
            End If
        Else
            If BLCtrl.Disconnect() Then
                ConnectDisconnectCmd.Text = "Connect"
                LogTextBox.AppendText("Disconnected" + vbCrLf)
                TestGroup.Enabled = False
            End If
        End If
    End Sub



    Private Sub ClearLogCmd_Click(sender As Object, e As EventArgs) Handles ClearLogCmd.Click
        LogTextBox.Clear()
        LogTextBox.Text = "[Log]" + vbCrLf
    End Sub



    Private Sub BLStep(CurrentStep As UInt16, TotalSteps As UInt16) Handles BLCtrl.onBootloaderStep
        'LogTextBox.AppendText(CurrentStep.ToString("D4") + "/" + TotalSteps.ToString() + vbCrLf)
        ProgressBar1.Maximum = TotalSteps
        ProgressBar1.Value = CurrentStep
    End Sub

    Private Sub Button9_Click(sender As Object, e As EventArgs) Handles Button9.Click
        Dim HexFile As String = ""

        Try
            HexFile = InputBox("Enter hex file full path", "Bootloader", "demo_app_1.0.0.hex")

            LogTextBox.AppendText("Loading file: " + HexFile + vbCrLf)

            If BLCtrl.UpdateFirmware(HexFile) Then
                LogTextBox.AppendText("Done" + vbCrLf)

                ConnectDisconnectCmd_Click(sender, e)   'disconnect

            Else
                LogTextBox.AppendText("ERROR" + vbCrLf)
            End If
        Catch ex As Exception

        End Try
    End Sub
End Class
