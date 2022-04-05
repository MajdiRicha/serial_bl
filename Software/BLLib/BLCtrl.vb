Imports System.IO.Ports
Imports System.Windows.Forms
Imports System.Threading

Public Class BLCtrl
    Private Enum CommIndex
        COMM_INDEX_LEN_L
        COMM_INDEX_LEN_H
        COMM_INDEX_FUNC
        COMM_INDEX_CMD
        COMM_INDEX_ARG0
        COMM_INDEX_ARG1
        COMM_INDEX_ARG2
        COMM_INDEX_ARG3
        COMM_INDEX_ARG4
        COMM_INDEX_ARG5
        COMM_INDEX_ARG6
        COMM_INDEX_ARG7
        COMM_INDEX_ARG8
    End Enum

    Private Enum CommFunction
        'analog functions
        COMM_CURR = &H4
        COMM_VOLT
        COMM_FREQ
        'logger functions
        COMM_LOG_COUNT
        COMM_LOG_DATA
        COMM_LOG_RATE
        'device functions
        COMM_FUNC_FW_REV = &H80
        COMM_FUNC_FW_DAT
        'Bootloader functions
        COMM_FUNC_FLASH = &HF0
    End Enum

    Private Enum CommCommand
        COMM_CMD_CONFIG
        COMM_CMD_INIT_RD
        COMM_CMD_RD
        COMM_CMD_INIT_WR
        COMM_CMD_WR
    End Enum


    'The port:
    Private CommSerialPort As New SerialPort

    'local global variables
    Private _DataReceived As Boolean = False

    'Comm
    Private Const COMM_MAX_BUFF_SIZE As UInt16 = 512
    Private Const COMM_BUFF_INDEX_MASK As UInt16 = (COMM_MAX_BUFF_SIZE - 1)
    Private Const COMM_REQUEST As Byte = &H80
    Private Const USB_CDC_TIMEOUT = 1000    'im ms
    Private RxBuff(0 To COMM_MAX_BUFF_SIZE - 1) As Byte

    'events
    Public Event onBootloaderStep(CurrentStep As UInt16, TotalSteps As UInt16)

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="COM"></param>
    ''' <returns></returns>
    Public Function Connect(COM As String) As Boolean

        Try
            With CommSerialPort
                .PortName = COM
                .DiscardNull = False
                .BaudRate = 115200
                .DataBits = 8
                .Parity = Parity.None
                .StopBits = StopBits.One
                .Handshake = Handshake.None
                .ReceivedBytesThreshold = 1
            End With
            CommSerialPort.Open()
        Catch ex As Exception
            Debug.Print(ex.Message)
            Return False
        End Try

        Return True
    End Function

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <returns></returns>
    Public Function Disconnect() As Boolean

        Try
            CommSerialPort.DiscardInBuffer()  'clear input buffer
            CommSerialPort.Close()
        Catch ex As Exception
            Debug.Print(ex.Message)
            Return False
        End Try

        Return True
    End Function

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="DtBuff"></param>
    ''' <param name="DtLen"></param>
    ''' <returns></returns>
    Private Function CommCalculateChecksum(ByRef DtBuff() As Byte, ByVal DtLen As UInt16) As Byte
        Dim i As UInt16
        Dim checksum As Int32 = 0
        Dim lsb As Byte

        Try
            For i = 0 To DtLen - 2
                checksum += DtBuff(i)
            Next

            checksum = -checksum
            lsb = BitConverter.GetBytes(checksum)(0)    'get LSB

        Catch ex As Exception
            Debug.Print(ex.Message)
        End Try

        Return lsb

    End Function
    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="DtBuff"></param>
    ''' <param name="DtLen"></param>
    ''' <returns></returns>
    Private Function CommChecksumOk(ByRef DtBuff() As Byte, ByVal DtLen As UInt16) As Boolean
        Dim i As UInt16
        Dim checksum As UShort = 0
        Try
            For i = 0 To DtLen - 1
                checksum += DtBuff(i)
            Next
            If CByte(checksum And &HFF) = 0 Then Return True
        Catch ex As Exception
            Debug.Print(ex.Message)
        End Try

        Return False
    End Function

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="TxBuffer"></param>
    ''' <param name="TxBufferOffset"></param>
    ''' <param name="TxBufferCount"></param>
    ''' <param name="RxBuffer"></param>
    ''' <param name="RxBufferOffset"></param>
    ''' <param name="RxBufferCount"></param>
    ''' <returns></returns>
    Private Function SerialPortTxRx(ByRef TxBuffer() As Byte, TxBufferOffset As UInt16, TxBufferCount As UInt16,
                                       ByRef RxBuffer() As Byte, RxBufferOffset As UInt16, RxBufferCount As UInt16) As Boolean
        Dim STime As DateTime
        Dim PTime As DateTime

        Try
            CommSerialPort.DiscardInBuffer()    'clear input buffer

            _DataReceived = False

            CommSerialPort.Write(TxBuffer, TxBufferOffset, TxBufferCount)

            STime = Now

            While (CommSerialPort.BytesToRead <> RxBufferCount)
                PTime = Now
                If (PTime - STime).TotalMilliseconds > USB_CDC_TIMEOUT Then Return False
            End While

            CommSerialPort.Read(RxBuffer, RxBufferOffset, RxBufferCount)

            Return True
        Catch ex As Exception
            Return False
        End Try
    End Function



    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="FwRev"></param>
    ''' <returns></returns>
    Public Function GetFirmwareRevision(ByRef FwRev As String) As Boolean
        Dim TxBuff() As Byte = {&H0, &H0, CommFunction.COMM_FUNC_FW_REV, (CommCommand.COMM_CMD_RD Or COMM_REQUEST), 0}
        Dim res As Boolean = False

        'construct request packet:
        TxBuff(CommIndex.COMM_INDEX_LEN_L) = BitConverter.GetBytes(TxBuff.Count)(0) 'Size L
        TxBuff(CommIndex.COMM_INDEX_LEN_H) = BitConverter.GetBytes(TxBuff.Count)(1) 'Size H
        TxBuff(TxBuff.Count - 1) = CommCalculateChecksum(TxBuff, TxBuff.Count)      'Checksum

        Try
            _DataReceived = SerialPortTxRx(TxBuff, CommIndex.COMM_INDEX_LEN_L, TxBuff.Count,
                            RxBuff, 0, 8)

            If _DataReceived And Not CBool((RxBuff(CommIndex.COMM_INDEX_CMD) And COMM_REQUEST)) Then
                FwRev = RxBuff(CommIndex.COMM_INDEX_ARG0).ToString + "." + RxBuff(CommIndex.COMM_INDEX_ARG1).ToString + "." + RxBuff(CommIndex.COMM_INDEX_ARG2).ToString
                res = True
            Else
                FwRev = "TIMEOUT"
            End If
        Catch ex As Exception
            Debug.Print(ex.Message)
        End Try

        Return res
    End Function
    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="FwDate"></param>
    ''' <returns></returns>
    Public Function GetFirmwareDate(ByRef FwDate As String) As Boolean
        Dim TxBuff() As Byte = {&H0, &H0, CommFunction.COMM_FUNC_FW_DAT, (CommCommand.COMM_CMD_RD Or COMM_REQUEST), 0}
        Dim res As Boolean = False

        'construct request packet:
        TxBuff(CommIndex.COMM_INDEX_LEN_L) = BitConverter.GetBytes(TxBuff.Count)(0) 'Size L
        TxBuff(CommIndex.COMM_INDEX_LEN_H) = BitConverter.GetBytes(TxBuff.Count)(1) 'Size H
        TxBuff(TxBuff.Count - 1) = CommCalculateChecksum(TxBuff, TxBuff.Count)      'Checksum

        Try
            _DataReceived = SerialPortTxRx(TxBuff, CommIndex.COMM_INDEX_LEN_L, TxBuff.Count,
                            RxBuff, 0, 8)

            If _DataReceived And Not CBool((RxBuff(CommIndex.COMM_INDEX_CMD) And COMM_REQUEST)) Then
                FwDate = RxBuff(CommIndex.COMM_INDEX_ARG0).ToString + "/" + RxBuff(CommIndex.COMM_INDEX_ARG1).ToString + "/" + RxBuff(CommIndex.COMM_INDEX_ARG2).ToString
                res = True
            Else
                FwDate = "TIMEOUT"
            End If
        Catch ex As Exception
            Debug.Print(ex.Message)
        End Try

        Return res
    End Function
    Private Function SetBootloaderMode(ByRef State As BootState) As Boolean
        Dim TxBuff() As Byte = {&H0, &H0, CommFunction.COMM_FUNC_FLASH, (CommCommand.COMM_CMD_CONFIG Or COMM_REQUEST), State, 0}
        Dim res As Boolean = False

        'construct request packet:
        TxBuff(CommIndex.COMM_INDEX_LEN_L) = BitConverter.GetBytes(TxBuff.Count)(0) 'Size L
        TxBuff(CommIndex.COMM_INDEX_LEN_H) = BitConverter.GetBytes(TxBuff.Count)(1) 'Size H
        TxBuff(TxBuff.Count - 1) = CommCalculateChecksum(TxBuff, TxBuff.Count)      'Checksum

        Try
            _DataReceived = SerialPortTxRx(TxBuff, CommIndex.COMM_INDEX_LEN_L, TxBuff.Count,
                            RxBuff, 0, 6)

            If _DataReceived And Not CBool((RxBuff(CommIndex.COMM_INDEX_CMD) And COMM_REQUEST)) Then
                State = RxBuff(CommIndex.COMM_INDEX_ARG0)
                res = True
            Else
                State = BootState.BOOT_INVALID
            End If
        Catch ex As Exception
            Debug.Print(ex.Message)
        End Try

        Return res
    End Function

    Private Function SendHexLine(ByVal HexEntry As HexFileEntry) As Boolean
        Dim TxBuff(0 To (9 + HexEntry.LineByteCount) - 1) As Byte
        Dim res As Boolean

        'construct request packet:
        TxBuff(CommIndex.COMM_INDEX_LEN_L) = BitConverter.GetBytes(TxBuff.Count)(0) 'Size L
        TxBuff(CommIndex.COMM_INDEX_LEN_H) = BitConverter.GetBytes(TxBuff.Count)(1) 'Size H
        TxBuff(CommIndex.COMM_INDEX_FUNC) = CommFunction.COMM_FUNC_FLASH
        TxBuff(CommIndex.COMM_INDEX_CMD) = CommCommand.COMM_CMD_WR Or COMM_REQUEST
        TxBuff(CommIndex.COMM_INDEX_ARG0) = HexEntry.LineType
        TxBuff(CommIndex.COMM_INDEX_ARG1) = HexEntry.LineByteCount
        TxBuff(CommIndex.COMM_INDEX_ARG2) = BitConverter.GetBytes(HexEntry.LineAddress)(0) 'address LSB
        TxBuff(CommIndex.COMM_INDEX_ARG3) = BitConverter.GetBytes(HexEntry.LineAddress)(1) 'address MSB
        'copy data
        Array.Copy(HexEntry.LineData, 0, TxBuff, CommIndex.COMM_INDEX_ARG4, HexEntry.LineByteCount)
        TxBuff(TxBuff.Count - 1) = CommCalculateChecksum(TxBuff, TxBuff.Count)      'Checksum

        Try
            _DataReceived = SerialPortTxRx(TxBuff, CommIndex.COMM_INDEX_LEN_L, TxBuff.Count,
                            RxBuff, 0, 6)

            If _DataReceived And Not CBool((RxBuff(CommIndex.COMM_INDEX_CMD) And COMM_REQUEST)) Then

                res = True
            Else

            End If
        Catch ex As Exception
            Debug.Print(ex.Message)
        End Try

        Return res
    End Function

    Public Function UpdateFirmware(ByVal HexFileName As String) As Boolean
        Dim HexFileEntries() As HexFileEntry = Nothing
        Dim index As UInt16
        Dim ret As Boolean = True

        Try
            If LoadHexFile(HexFileName, HexFileEntries) Then
                If SetBootloaderMode(BootState.BOOT_LOADER) Then
                    Thread.Sleep(100)   'Wait a bit
                    For index = 0 To HexFileEntries.Length - 1
                        ret = ret And SendHexLine(HexFileEntries(index))
                        If ret Then
                            RaiseEvent onBootloaderStep(index + 1, HexFileEntries.Length)
                            Application.DoEvents()
                        Else
                            Return False
                        End If
                    Next
                    If ret Then
                        If SetBootloaderMode(BootState.BOOT_APP) Then

                        Else
                            Return False
                        End If
                    Else
                        Return False
                    End If
                Else
                    Return False
                End If
            Else
                Return False
            End If
        Catch ex As Exception
            Return False
        End Try

        Return True
    End Function
End Class
