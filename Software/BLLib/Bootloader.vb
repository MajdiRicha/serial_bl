Imports System.IO
Imports System.Text
Imports System.Threading

Module Bootloader
    Public Enum BootState
        BOOT_LOADER
        BOOT_APP
        BOOT_INVALID
    End Enum
    Public Structure HexFileEntry
        Public LineType As Byte
        Public LineByteCount As Byte
        Public LineAddress As UShort
        Public LineData() As Byte
    End Structure
    Const dummy = 0
    'Public Function SetBootloaderMode(ByVal SHABmodule As SHABDevice, ByVal ModuleInstance As Byte, ByVal State As BootState) As Boolean
    '    Dim SHABbuff() As Byte = {9, SHABDevice.EC_D, dummy, SHABmodule, ModuleInstance, SHABCommand.SHAB_CW Or SHAB_REQUEST, SHABUniversalFunc.SHAB_FLASH_F, State}
    '    Dim inBuff(0 To MAXRXBYTES - 1) As Byte
    '    Dim ret As Boolean, DataReady As Boolean
    '    Dim OuterRetries As Byte = 2, InnerRetries As Byte
    '    Dim Done As Boolean = False

    '    Do
    '        ret = SetSerial(SHABbuff, SHABbuff.Length)
    '        Thread.Sleep(50)
    '        InnerRetries = 3

    '        Do
    '            ret = ret And SerialReady(DataReady)
    '            If DataReady Then
    '                ret = ret And GetSerial(inBuff)
    '                If isACK(inBuff, SHABmodule, ModuleInstance, SHABUniversalFunc.SHAB_FLASH_F, SHABCommand.SHAB_CW) Then
    '                    'todo:

    '                    Done = True
    '                End If
    '            Else
    '                Thread.Sleep(10)
    '            End If

    '            InnerRetries = InnerRetries - 1
    '        Loop While Not Done And (InnerRetries > 0)

    '        OuterRetries = OuterRetries - 1
    '    Loop While Not Done And (OuterRetries > 0)

    '    If Done Then
    '        SetBootloaderMode = ret
    '    Else
    '        SetBootloaderMode = False
    '    End If
    'End Function
    'Public Function SendHexLine(ByVal SHABmodule As SHABDevice, ByVal ModuleInstance As Byte, ByVal HexEntry As HexFileEntry) As Boolean
    '    Dim SHABbuff(0 To (HexEntry.LineByteCount + 11) - 1) As Byte
    '    Dim inBuff(0 To MAXRXBYTES - 1) As Byte
    '    Dim ret As Boolean, DataReady As Boolean
    '    Dim OuterRetries As Byte = 2, InnerRetries As Byte
    '    Dim Done As Boolean = False

    '    SHABbuff(SHABLocation.SHAB_MSG_LENGTH) = HexEntry.LineByteCount + 12 '(including checksum)
    '    SHABbuff(SHABLocation.SHAB_SOURCE_ID) = SHABDevice.EC_D
    '    SHABbuff(SHABLocation.SHAB_SOURCE_INSTANCE) = dummy
    '    SHABbuff(SHABLocation.SHAB_DESTINATION_ID) = SHABmodule
    '    SHABbuff(SHABLocation.SHAB_DESTINATION_INSTANCE) = ModuleInstance
    '    SHABbuff(SHABLocation.SHAB_COMMAND) = SHABCommand.SHAB_W Or SHAB_REQUEST
    '    SHABbuff(SHABLocation.SHAB_FUNCTION) = SHABUniversalFunc.SHAB_FLASH_F
    '    SHABbuff(SHABLocation.SHAB_PARAM + 0) = HexEntry.LineType
    '    SHABbuff(SHABLocation.SHAB_PARAM + 1) = HexEntry.LineByteCount
    '    SHABbuff(SHABLocation.SHAB_PARAM + 2) = CByte(HexEntry.LineAddress And &HFF) 'address LSB
    '    SHABbuff(SHABLocation.SHAB_PARAM + 3) = CByte(Fix(HexEntry.LineAddress / 256)) 'address MSB

    '    'copy data
    '    Array.Copy(HexEntry.LineData, 0, SHABbuff, SHABLocation.SHAB_PARAM + 4, HexEntry.LineByteCount)

    '    Do
    '        ret = SetSerial(SHABbuff, SHABbuff.Length)
    '        Thread.Sleep(50)
    '        InnerRetries = 3

    '        Do
    '            ret = ret And SerialReady(DataReady)
    '            If DataReady Then
    '                ret = ret And GetSerial(inBuff)
    '                If CheckSumOk(inBuff) And inBuff(SHABLocation.SHAB_MSG_LENGTH) = 12 _
    '                And inBuff(SHABLocation.SHAB_SOURCE_ID) = SHABmodule And inBuff(SHABLocation.SHAB_SOURCE_INSTANCE) = ModuleInstance _
    '                And inBuff(SHABLocation.SHAB_COMMAND) = SHABCommand.SHAB_W _
    '                And inBuff(SHABLocation.SHAB_PARAM + 2) = SHABbuff(SHABLocation.SHAB_PARAM + 2) _
    '                And inBuff(SHABLocation.SHAB_PARAM + 3) = SHABbuff(SHABLocation.SHAB_PARAM + 3) Then
    '                    'todo:

    '                    Done = True
    '                End If
    '            Else
    '                Thread.Sleep(10)
    '            End If

    '            InnerRetries = InnerRetries - 1
    '        Loop While Not Done And (InnerRetries > 0)

    '        OuterRetries = OuterRetries - 1
    '    Loop While Not Done And (OuterRetries > 0)

    '    If Done Then
    '        SendHexLine = ret
    '    Else
    '        SendHexLine = False
    '    End If
    'End Function
    Public Function LoadHexFile(ByVal HexFile As String, ByRef HexEntries() As HexFileEntry) As Boolean
        Dim EntryCount As UShort
        Dim i As Byte

        Try
            Dim FileArray As String() = File.ReadAllLines(HexFile)
            Dim FileLine As String
            For Each FileLine In FileArray
                If FileLine.StartsWith(":") Then
                    ReDim Preserve HexEntries(EntryCount)
                    HexEntries(EntryCount).LineByteCount = CByte("&H" + FileLine.Substring(1, 2))
                    HexEntries(EntryCount).LineAddress = CUShort("&H" + FileLine.Substring(3, 4))
                    HexEntries(EntryCount).LineType = CUShort("&H" + FileLine.Substring(7, 2))
                    If HexEntries(EntryCount).LineType <> 1 Then
                        'Data bytes:
                        ReDim HexEntries(EntryCount).LineData(0 To HexEntries(EntryCount).LineByteCount - 1)
                        For i = 0 To HexEntries(EntryCount).LineByteCount - 1
                            HexEntries(EntryCount).LineData(i) = CByte("&H" + FileLine.Substring(9 + i * 2, 2))
                        Next
                        EntryCount = EntryCount + 1
                    End If
                End If
            Next
            ReDim Preserve HexEntries(EntryCount - 1)   'remove EOF entry
            LoadHexFile = True
        Catch ex As Exception
            LoadHexFile = False
        End Try
    End Function
End Module
