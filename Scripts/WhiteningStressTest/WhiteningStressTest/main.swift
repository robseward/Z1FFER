//
//  main.swift
//  WhiteningStressTest
//
//  Created by Rob Seward on 2/22/15.
//  Copyright (c) 2015 Rob Seward. All rights reserved.
//

import Foundation



let SampleLength: UInt = 400000




func createPopcountLookupTable() -> [Int]{
    var table = [Int](count: 256, repeatedValue: 0)
    for var i = 0; i < 256; i++ {
        table[i] = (i & 1) + table[i/2]
    }
    return table
}

func initializeRandomArray( inout randomArray : [UInt8]){
    for var i=0; i < Int(SampleLength); i++ {
        randomArray.append( UInt8(arc4random_uniform(256)) )
    }
}

func calculateDeviation(inout randomArray : [UInt8]) -> Float{
    let bitCountLookupTable = createPopcountLookupTable()
    var count : Int = 0
    for v in randomArray {
        count = bitCountLookupTable[Int(v)] + count
    }
    let idealCount : Int = (randomArray.count * 8) / 2
    let ideal : Float = 0.5
    //println(count)
    return Float(count - idealCount) / Float(idealCount)
}

func adjustDeviation(inout randomArray : [UInt8], targetDeviation : Float){
    var deviation : Float = 0.0
    while fabs(deviation) < fabs(targetDeviation) {
        for var i=0; i < 10; i++ {
            let randomIndex = Int(arc4random_uniform(UInt32(SampleLength)))
            var selectedByte = randomArray[randomIndex]
            let shiftBy = UInt8(arc4random_uniform(8))
            if targetDeviation > 0 {
                selectedByte = selectedByte | (0x01 << shiftBy)
            }
            else {
                selectedByte = selectedByte & ~(0x01 << shiftBy)
            }
            randomArray[randomIndex] = selectedByte
        }
        deviation = calculateDeviation(&randomArray)
        //println(fabs(deviation))
    }
}

func whiten(inout randomArray : [UInt8]){
    var previousByte = UInt8(0)
    var previousMixedByte1 = UInt8(0)
    var previousMixedByte2 = UInt8(0)
    var previousMixedByte3 = UInt8(0)
    var previousMixedByte4 = UInt8(0)
    
    for var i=0; i < randomArray.count; i++ {
        let b = randomArray[i]
        let mixedByte1 = b ^ previousByte
        let mixedByte2 = mixedByte1 ^ previousMixedByte1
        let mixedByte3 = mixedByte2 ^ previousMixedByte2
        let mixedByte4 = mixedByte3 ^ previousMixedByte3
        let outByte = mixedByte4 ^ previousMixedByte4
        
        randomArray[i] = outByte
        
        previousMixedByte1 = mixedByte1
        previousMixedByte2 = mixedByte2
        previousMixedByte3 = mixedByte3
        previousMixedByte4 = mixedByte4
    }
}

func saveToFile(filename : String, inout randomArray : [UInt8]){
    let dirs : [String]? = NSSearchPathForDirectoriesInDomains(NSSearchPathDirectory.DesktopDirectory, NSSearchPathDomainMask.AllDomainsMask, true) as? [String]
    if ((dirs) != nil) {
        let dir = dirs![0]; //desktop directory
        let path = dir.stringByAppendingPathComponent(filename);
        let data = NSData(bytes: &randomArray, length: randomArray.count)
        println(path)
        data.writeToFile(path, atomically: true);
    }
}

func main() {
    
    var randomArray = [UInt8]()
    initializeRandomArray(&randomArray)
    let filename = "random_baseline.dat"
    saveToFile(filename, &randomArray)
    let queue = NSOperationQueue()
    for var deviation:Float = -0.1; deviation < 0.1; deviation += 0.01{
        let staticDeviation = deviation
        queue.addOperationWithBlock({ () -> Void in
            var randomArray = [UInt8]()
            initializeRandomArray(&randomArray)
            adjustDeviation(&randomArray, staticDeviation)
            whiten(&randomArray)
            let whitenedDeviation = calculateDeviation(&randomArray)
            println("\nDeviation: \(staticDeviation) Whitened = \(whitenedDeviation)")
            let filename = "random_test_\(staticDeviation).dat"
            saveToFile(filename, &randomArray)
        })
    }

    queue.waitUntilAllOperationsAreFinished()

}


let startTime = NSDate()

main()

println("\nElapsed Time: \(startTime.timeIntervalSinceNow)")















